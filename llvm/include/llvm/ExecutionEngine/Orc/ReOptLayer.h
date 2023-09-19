//===- ReOptLayer.h - Re-optimization layer interface -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Re-optimization layer interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_EXECUTIONENGINE_ORC_REOPTLAYER_H
#define LLVM_EXECUTIONENGINE_ORC_REOPTLAYER_H

#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/Orc/Layer.h"
#include "llvm/ExecutionEngine/Orc/RedirectionManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"


namespace llvm {
namespace orc {

class ReOptLayer : public IRLayer {
public: 
  using MaterializationUnitID = uint64_t;
  using AddProfilerFunc = unique_function<Error (ReOptLayer& Parent, MaterializationUnitID MUID, ThreadSafeModule& TSM)>;
  using ReOptimizeFunc = unique_function<void (ReOptLayer& Parent, ResourceTrackerSP OldRT, ThreadSafeModule& TSM)>;
public:
  ReOptLayer(ExecutionSession& ES, IRLayer& BaseLayer, RedirectableSymbolManager& RM) 
    : IRLayer(ES, BaseLayer.getManglingOptions()), ES(ES), BaseLayer(BaseLayer), RSManager(RM)
  {}

  Error reigsterRuntimeFunctions(JITDylib& JD) {
    ExecutionSession::JITDispatchHandlerAssociationMap WFs;
    using ReoptimizeSPSSig =
        shared::SPSError(uint64_t, uint32_t);
    WFs[ES.intern("__orc_rt_reoptimize_tag")] =
        ES.wrapAsyncWithSPS<ReoptimizeSPSSig>(this,
                                                &ReOptLayer::rt_reoptimize);
    return ES.registerJITDispatchHandlers(JD, std::move(WFs));
  }

  /// Emits the given module. This should not be called by clients: it will be
  /// called by the JIT when a definition added via the add method is requested.
  void emit(std::unique_ptr<MaterializationResponsibility> R,
            ThreadSafeModule TSM) override {
    auto& JD = R->getTargetJITDylib();

    bool HasNonCallable = false;
    for (auto &KV : R->getSymbols()) {
      auto &Flags = KV.second;
      if (!Flags.isCallable())
        HasNonCallable = true;
    }

    if (HasNonCallable) {
      BaseLayer.emit(std::move(R), std::move(TSM)); 
      return;
    }

    MaterializationUnitID MUID = NextID;
    MUStates.emplace(MUID, MaterializationUnitState(MUID, cloneToNewContext(TSM)));
    ++NextID;

    if (auto Err = reoptimizeIfCallFrequent(*this, MUID, TSM)) {
      ES.reportError(std::move(Err));
      R->failMaterialization();
      return;
    }

    auto RenamedMap = renameSymbolsToImpl(MUStates.at(MUID), TSM);
    if (auto Err = RenamedMap.takeError()) {
      ES.reportError(std::move(Err));
      R->failMaterialization();
      return;
    }

    auto RT = JD.createResourceTracker();
    if (auto Err = JD.define(
            std::make_unique<BasicIRLayerMaterializationUnit>(
                BaseLayer, *getManglingOptions(), std::move(TSM)), RT)) {
      ES.reportError(std::move(Err));
      R->failMaterialization();
      return;
    }
    MUStates.at(MUID).RT = RT;

    SymbolLookupSet LookupSymbols;
    for (auto [K,V] : *RenamedMap)
      LookupSymbols.add(V);

    auto ImplSymbols = ES.lookup({{&JD, JITDylibLookupFlags::MatchAllSymbols}}, LookupSymbols, LookupKind::Static, SymbolState::Resolved);
    if (auto Err = ImplSymbols.takeError()) {
      ES.reportError(std::move(Err));
      R->failMaterialization();
      return;
    }

    SymbolMap InitialDests;
    for (auto [K,V] : *RenamedMap)
      InitialDests[K] = (*ImplSymbols)[V];

    RSManager.emitRedirectableSymbols(std::move(R), std::move(InitialDests));
  }

  static const uint64_t CallCountThreshold = 10;
  static Error reoptimizeIfCallFrequent(ReOptLayer& Parent, MaterializationUnitID MUID, ThreadSafeModule& TSM) { 
    return TSM.withModuleDo([&](Module &M) -> Error {
      Type* I64Ty = Type::getInt64Ty(M.getContext());
      GlobalVariable* Counter = new GlobalVariable(M, I64Ty, false, GlobalValue::InternalLinkage, Constant::getNullValue(I64Ty), "__orc_reopt_counter");
      auto ArgBufferConst = createReoptimizeArgBuffer(M, MUID, Parent.MUStates[MUID].CurVersion);
      if (auto Err = ArgBufferConst.takeError()) 
          return Err;
      GlobalVariable* ArgBuffer = new GlobalVariable(M, (*ArgBufferConst)->getType(), true, GlobalValue::InternalLinkage, (*ArgBufferConst));
      for (auto& F : M) {
        if (F.isDeclaration()) 
          continue;
        auto& BB = F.getEntryBlock();
        dbgs() << BB << "\n";
        auto* IP = &*BB.getFirstInsertionPt();
        IRBuilder<> IRB(IP);
        Value* Threshold = ConstantInt::get(I64Ty, CallCountThreshold, true);
        Value* Cnt = IRB.CreateLoad(I64Ty, Counter);
        // Use EQ to prevent further reoptimize calls.
        Value* Cmp = IRB.CreateICmpEQ(Cnt, Threshold);
        Value* Added = IRB.CreateAdd(Cnt, ConstantInt::get(I64Ty, 1));
        (void)IRB.CreateStore(Added, Counter);
        Instruction* SplitTerminator = SplitBlockAndInsertIfThen(Cmp, IP, false);
        createCallReoptimizeBlock(M, *SplitTerminator, ArgBuffer);
        dbgs() << M << "\n";
      }
      return Error::success();
    });
  }
private:
  struct MaterializationUnitState {
    MaterializationUnitState() = default;
    MaterializationUnitState(MaterializationUnitID ID, ThreadSafeModule TSM) : ID(ID), TSM(std::move(TSM)) {}
    MaterializationUnitState(MaterializationUnitState&& Other) : ID(Other.ID), TSM(std::move(Other.TSM)), RT(std::move(Other.RT)), Compiling(std::move(Other.Compiling)), CurVersion(Other.CurVersion) {}
    std::mutex Mutex;
    MaterializationUnitID ID;
    ThreadSafeModule TSM;
    ResourceTrackerSP RT;
    bool Compiling = false;
    uint32_t CurVersion = 0;
  };

  using SPSReoptimizeArgList = shared::SPSArgList<MaterializationUnitID, uint32_t>;
  using SendErrorFn = unique_function<void(Error)>;

  static void Func2() {
    printf("Reoptimized\n");
  }

  void rt_reoptimize(SendErrorFn SendResult, MaterializationUnitID MUID, uint32_t CurVersion) {
    dbgs() << "Reoptimize Requested" << "\n";
    SymbolMap Redirected;
    Redirected[ES.intern("f")] = ExecutorSymbolDef(ExecutorAddr((uint64_t)&Func2), JITSymbolFlags::Callable);
    cantFail(RSManager.redirect(MUStates[MUID].RT->getJITDylib(), Redirected));
    /* auto& MUState = MUStates[MUID]; */
    /* if (CurVersion < MUState.CurVersion || MUState.Compiling) { */
    /*   SendResult(Error::success()); */
    /*   return; */
    /* } */
    /* MUState.CurVersion++; */
    /* MUState.Compiling = true; */
    /* ThreadSafeModule TSM = cloneToNewContext(MUState.TSM); */
    /* CurVersion++; */
    /* auto Callback = [this, MUID, CurVersion](ThreadSafeModule TSM) { */
    /*   defineImpl(MUID, CurVersion, std::move(TSM)); */
    /* }; */
  }

  using RenamedSymbolMap = DenseMap<SymbolStringPtr, SymbolStringPtr>;
  Expected<RenamedSymbolMap> renameSymbolsToImpl(MaterializationUnitState& State, ThreadSafeModule& TSM) {
    RenamedSymbolMap RenamedMap;
    cantFail(TSM.withModuleDo([&](Module& M) -> Error {
    MangleAndInterner Mangle(ES, M.getDataLayout());
      for (auto& F : M)
        if (!F.isDeclaration()) {
          std::string NewName = (F.getName() + ".__def." + Twine(State.CurVersion)).str();
          RenamedMap[Mangle(F.getName())] = Mangle(NewName);
          F.setName(NewName);
        }
      return Error::success();
    }));
    return RenamedMap;
  }

  static Expected<Constant*> createReoptimizeArgBuffer(Module& M, MaterializationUnitID MUID, uint32_t CurVersion) {
    size_t ArgBufferSize = SPSReoptimizeArgList::size(MUID, CurVersion);
    std::vector<char> ArgBuffer(ArgBufferSize);
    shared::SPSOutputBuffer OB(ArgBuffer.data(), ArgBuffer.size());
    if (!SPSReoptimizeArgList::serialize(OB, MUID, CurVersion))
      return make_error<StringError>("Could not serealize args list", inconvertibleErrorCode());
    return ConstantDataArray::get(M.getContext(), ArrayRef(ArgBuffer));
  }

  static void createCallReoptimizeBlock(Module& M, Instruction& IP, GlobalVariable* ArgBuffer) {
    GlobalVariable* DispatchCtx = M.getGlobalVariable("__orc_rt_jit_dispatch_ctx");
    if (!DispatchCtx)
      DispatchCtx = new GlobalVariable(M, Type::getInt8PtrTy(M.getContext()), false, GlobalValue::ExternalLinkage, nullptr, "__orc_rt_jit_dispatch_ctx");
    GlobalVariable* ReoptimizeTag = M.getGlobalVariable("__orc_rt_reoptimize_tag");
    if (!ReoptimizeTag)
      ReoptimizeTag = new GlobalVariable(M, Type::getInt8PtrTy(M.getContext()), false, GlobalValue::ExternalLinkage, nullptr, "__orc_rt_reoptimize_tag");
    Function* DispatchFunc = M.getFunction("__orc_rt_jit_dispatch");
    if (!DispatchFunc) {
      std::vector<Type*> Args = { Type::getInt8PtrTy(M.getContext()), 
        Type::getInt8PtrTy(M.getContext()), Type::getInt8PtrTy(M.getContext()), IntegerType::get(M.getContext(), 64)};
      FunctionType* FuncTy = FunctionType::get(Type::getVoidTy(M.getContext()), Args, false);
      DispatchFunc = Function::Create(FuncTy, GlobalValue::ExternalLinkage, "__orc_rt_jit_dispatch", &M);
    }
    size_t ArgBufferSizeConst = SPSReoptimizeArgList::size(MaterializationUnitID{}, uint32_t{});
    Constant* ArgBufferSize = ConstantInt::get(IntegerType::get(M.getContext(), 64), ArgBufferSizeConst, false);
    IRBuilder<> IRB(&IP);
    (void)IRB.CreateCall(DispatchFunc, {DispatchCtx, ReoptimizeTag, ArgBuffer, ArgBufferSize});
  }

  ExecutionSession& ES;
  IRLayer &BaseLayer;
  RedirectableSymbolManager &RSManager;
  std::map<MaterializationUnitID, MaterializationUnitState> MUStates;
  MaterializationUnitID NextID = 1;
};

/* class ReOptIRMaterializationUnit : public IRMaterializationUnit { */
/* public: */
/*   ReOptIRMaterializationUnit(ExecutionSession &ES, */
/*                                     const IRSymbolMapper::ManglingOptions &MO, */
/*                                     ThreadSafeModule TSM, */
/*                                     ReOptLayer& Parent) */
/*       : IRMaterializationUnit(ES, MO, std::move(TSM)), Parent(Parent) {} */
/**/
/*   ReOptIRMaterializationUnit( */
/*       ThreadSafeModule TSM, Interface I, */
/*       SymbolNameToDefinitionMap SymbolToDefinition, ReOptLayer&Parent) */
/*       : IRMaterializationUnit(std::move(TSM), std::move(I), */
/*                               std::move(SymbolToDefinition)), */
/*         Parent(Parent) {} */
/**/
/* private: */
/*   void materialize(std::unique_ptr<MaterializationResponsibility> R) override { */
/*     Parent.emit(std::move(R), std::move(TSM)); */
/*   } */
/**/
/*   void discard(const JITDylib &V, const SymbolStringPtr &Name) override { */
/*     // All original symbols were materialized by the CODLayer and should be */
/*     // final. The function bodies provided by M should never be overridden. */
/*     llvm_unreachable("Discard should never be called on an " */
/*                      "ExtractingIRMaterializationUnit"); */
/*   } */
/**/
/*   ReOptLayer&Parent; */
/* }; */
/**/

}
}

#endif
