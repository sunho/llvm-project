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

  /// Emits the given module. This should not be called by clients: it will be
  /// called by the JIT when a definition added via the add method is requested.
  void emit(std::unique_ptr<MaterializationResponsibility> R,
            ThreadSafeModule TSM) override {
    auto& JD = R->getTargetJITDylib();

    bool HasNonCallable = false;
    for (auto &KV : R->getSymbols()) {
      auto &Name = KV.first;
      auto &Flags = KV.second;
      if (Flags.isCallable())
        Callables[Name] = SymbolAliasMapEntry(Name, Flags);
      else
        HasNonCallable = true;
    }

    if (HasNonCallable) {
      BaseLayer.emit(std::move(R), std::move(TSM)); 
      return;
    }

    R->replace(RedirectableMaterializationUnit())
  }

  Instruction* createReOptimizeCall(BasicBlock& BB, MaterializationUnitID MUID) {
    IRBuilder<> Builder(&BB);
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
        BasicBlock* CallBB = createCallReoptimizeBlock(M, &F, ArgBuffer);
        auto& BB = F.getEntryBlock();
        auto* IP = &*BB.getFirstInsertionPt();
        IRBuilder<> IRB(IP);
        Value* Threshold = ConstantInt::get(I64Ty, CallCountThreshold, true);
        Value* Cnt = IRB.CreateLoad(I64Ty, Counter);
        // Use EQ to prevent further reoptimize calls.
        Value* Cmp = IRB.CreateICmpEQ(Cnt, Threshold);
        Value* Added = IRB.CreateAdd(Cnt, ConstantInt::get(I64Ty, 1));
        (void)IRB.CreateStore(Added, Counter);
        (void)SplitBlockAndInsertIfThen(Cmp, IP, false, nullptr, nullptr, nullptr, CallBB);
      }
      return Error::success();
    });
  }
private:
  struct MaterializationUnitState {
    std::mutex Mutex;
    MaterializationUnitID ID;
    DenseSet<uint32_t> VersionsInQueue;
    uint32_t CurVersion;
    ThreadSafeModule TSM;
  };

  using SPSReoptimizeArgList = shared::SPSArgList<MaterializationUnitID, uint32_t>;
  using SendErrorFn = unique_function<void(Error)>;

  void rt_reoptimize(SendErrorFn SendResult, MaterializationUnitID MUID, uint32_t CurVersion) {
    auto& MUState = MUStates[MUID];
    if (CurVersion < MUState.CurVersion || MUState.VersionsInQueue.count(CurVersion)) {
      SendResult(Error::success());
      return;
    }
    MUState.CurVersion++;
    ThreadSafeModule TSM = cloneToNewContext(MUState.TSM);
    CurVersion++;
    auto Callback = [this, MUID, CurVersion](ThreadSafeModule TSM) {
      defineImpl(MUID, CurVersion, std::move(TSM));
    };
  }

  void defineImpl(MaterializationUnitID MUID, uint32_t CurVersion, ThreadSafeModule TSM) {
    std::vector<SymbolStringPtr> ToLookup;
    TSM.withModuleDo([&](Module& M) -> Error {
      for (auto& F : M)
        if (!F.isDeclaration())
          F.setName(F.getName() + ".__def." + Twine(CurVersion));
      return Error::success();
    });
    
    JD.define(std::make_unique<BasicIRLayerMaterializationUnit>(BaseLayer, std::move(TSM));
    ES.lookup({JD});
    if (First)
      RSManager.createRedirectableSymbol();
    else
      RSManager.redirect();
  }

  static Expected<Constant*> createReoptimizeArgBuffer(Module& M, MaterializationUnitID MUID, uint32_t CurVersion) {
    size_t ArgBufferSize = SPSReoptimizeArgList::size(MUID, CurVersion);
    std::vector<char> ArgBuffer(ArgBufferSize);
    shared::SPSOutputBuffer OB(ArgBuffer.data(), ArgBuffer.size());
    if (!SPSReoptimizeArgList::serialize(OB, MUID, CurVersion))
      return make_error<StringError>("Could not serealize args list", inconvertibleErrorCode());
    return ConstantDataArray::get(M.getContext(), ArrayRef(ArgBuffer));
  }

  static BasicBlock* createCallReoptimizeBlock(Module& M, Function* F, GlobalVariable* ArgBuffer) {
    BasicBlock *BB = BasicBlock::Create(F->getContext(), "OrcCallReoptimizeBlk", F);
    GlobalVariable* DispatchCtx = M.getGlobalVariable("__orc_rt_jit_dispatch_ctx");
    if (!DispatchCtx)
      DispatchCtx = new GlobalVariable(Type::getInt8PtrTy(M.getContext()), false, GlobalValue::ExternalLinkage, nullptr, "__orc_rt_jit_dispatch_ctx");
    GlobalVariable* ReoptimizeTag = M.getGlobalVariable("__orc_rt_reoptimize_tag");
    if (!ReoptimizeTag)
      ReoptimizeTag = new GlobalVariable(Type::getInt8PtrTy(M.getContext()), false, GlobalValue::ExternalLinkage, nullptr, "__orc_rt_reoptimize_tag");
    Function* DispatchFunc = M.getFunction("__orc_rt_jit_dispatch");
    if (!DispatchFunc) {
      std::vector<Type*> Args = { Type::getInt8PtrTy(M.getContext()), 
        Type::getInt8PtrTy(M.getContext()), Type::getInt8PtrTy(M.getContext()), IntegerType::get(M.getContext(), 64)};
      FunctionType* FuncTy = FunctionType::get(Type::getVoidTy(M.getContext()), Args, false);
      DispatchFunc = Function::Create(FuncTy, GlobalValue::ExternalLinkage, "__orc_rt_jit_dispatch", &M);
    }
    size_t ArgBufferSizeConst = SPSReoptimizeArgList::size(MaterializationUnitID{}, uint32_t{});
    Constant* ArgBufferSize = ConstantInt::get(IntegerType::get(M.getContext(), 64), ArgBufferSizeConst, false);
    IRBuilder<> IRB(BB);
    (void)IRB.CreateCall(DispatchFunc, {DispatchCtx, ReoptimizeTag, ArgBuffer, ArgBufferSize});
    return BB;
  }

  IRLayer &BaseLayer;
  RedirectableSymbolManager &RSManager;
  std::map<MaterializationUnitID, MaterializationUnitState> MUStates;
};

class RedirectableMaterializationUnit : public MaterializationUnit {
public:
  RedirectableMaterializationUnit(RedirectableSymbolManager& RSManager,
                                   JITDylib &SourceJD,
                                   SymbolAliasMap CallableAliases);
  

  StringRef getName() const override;

private:
  using GlobalValueSet = std::set<const GlobalValue *>;
  void materialize(std::unique_ptr<MaterializationResponsibility> R) override {
    auto &ES = R->getExecutionSession();

    auto RenamedSymbols = renameSymbols(TSM);
    JD.define(std::make_unique<BasicIRLayerMaterializationUnit>(TSM));
    R->replace(std::make_unique<RedirectableMaterializationUnit>(SymbolToRenamedSymbolMap));
    

    // No registered dependencies, so these calls cannot fail.
    cantFail(R->notifyResolved(Stubs));
    cantFail(R->notifyEmitted());
  }
  void discard(const JITDylib &JD, const SymbolStringPtr &Name) override {
    
  }
  static MaterializationUnit::Interface
  extractFlags(const SymbolAliasMap &Aliases) {

  }

  ReOptLayer &Parent;
  JITDylib &SourceJD;
  SymbolAliasMap CallableAliases;
};


class ReOptMaterializationUnit : public IRMaterializationUnit {
public:
  ReOptMaterializationUnit(RedirectableSymbolManager& RSManager,
                                   JITDylib &SourceJD,
                                   SymbolAliasMap CallableAliases);

  StringRef getName() const override;

private:
  using GlobalValueSet = std::set<const GlobalValue *>;
  void materialize(std::unique_ptr<MaterializationResponsibility> R) override {
    auto &ES = R->getExecutionSession();
    GlobalValueSet RequestedGVs;
    for (auto &[Name, Flags] : R->getSymbols()) {
      if (Name == R->getInitializerSymbol())
        return;
      else {
        assert(Defs.count(Name) && "No definition for symbol");
        RequestedGVs.insert(SymbolToDefinition[Name]);
      }
    }
    
  }
  void discard(const JITDylib &JD, const SymbolStringPtr &Name) override {
    
  }
  static MaterializationUnit::Interface
  extractFlags(const SymbolAliasMap &Aliases) {

  }

  ReOptLayer &Parent;
  JITDylib &SourceJD;
  SymbolAliasMap CallableAliases;
};

}
}

#endif
