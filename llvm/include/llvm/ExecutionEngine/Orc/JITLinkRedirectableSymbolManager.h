//===- JITLinkRedirectableSymbolManager.h - JITLink redirection -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Redirectable Symbol Manager implementation using JITLink
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_EXECUTIONENGINE_ORC_JITLINKREDIRECABLEMANAGER_H
#define LLVM_EXECUTIONENGINE_ORC_JITLINKREDIRECABLEMANAGER_H

#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/RedirectionManager.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/ExecutionEngine/JITLink/x86_64.h"

namespace llvm {
namespace orc {

class JITLinkRedirectableSymbolManager : public RedirectableSymbolManager,
                                         public ResourceManager {
public:
  /// Create redirection manager that uses JITLink based implementaion.
  static Expected<std::unique_ptr<RedirectableSymbolManager>>
  Create(ExecutionSession &ES, ObjectLinkingLayer &ObjLinkingLayer,
         JITDylib &JD) {
    Error Err = Error::success();
    auto RM = std::unique_ptr<RedirectableSymbolManager>(
        new JITLinkRedirectableSymbolManager(ES, ObjLinkingLayer, JD, Err));
    if (Err)
      return Err;
    return std::move(RM);
  }

  void emitRedirectableSymbols(std::unique_ptr<MaterializationResponsibility> R,
                               const SymbolAddrMap &InitialDests) override;

  Error redirect(JITDylib &TargetJD, const SymbolAddrMap &NewDests) override;

  Error handleRemoveResources(JITDylib &TargetJD, ResourceKey K) override;

  void handleTransferResources(JITDylib &TargetJD, ResourceKey DstK,
                               ResourceKey SrcK) override;

private:
  using StubHandle = unsigned;
  constexpr static unsigned StubBlockSize = 256;
  constexpr static StringRef JumpStubPrefix = "$__IND_JUMP_STUBS";
  constexpr static StringRef StubPtrPrefix = "$IND_JUMP_PTR_";
  constexpr static StringRef JumpStubTableName = "$IND_JUMP_";
  constexpr static StringRef StubPtrTableName = "$__IND_JUMP_PTRS";

  JITLinkRedirectableSymbolManager(ExecutionSession &ES,
                                   ObjectLinkingLayer &ObjLinkingLayer,
                                   JITDylib &JD, Error &Err)
      : ES(ES), ObjLinkingLayer(ObjLinkingLayer), JD(JD),
        AnonymousPtrCreator(
            jitlink::getAnonymousPointerCreator(ES.getTargetTriple())),
        PtrJumpStubCreator(
            jitlink::getPointerJumpStubCreator(ES.getTargetTriple())) {
    if (!AnonymousPtrCreator || !PtrJumpStubCreator)
      Err = make_error<StringError>("Architecture not supported",
                                    inconvertibleErrorCode());
    if (Err)
      return;
    ES.registerResourceManager(*this);
  }

  ~JITLinkRedirectableSymbolManager() { ES.deregisterResourceManager(*this); }

  StringRef JumpStubSymbolName(unsigned I) {
    return *ES.intern((JumpStubPrefix + Twine(I)).str());
  }

  StringRef StubPtrSymbolName(unsigned I) {
    return *ES.intern((StubPtrPrefix + Twine(I)).str());
  }

  unsigned GetNumAvailableStubs() const { return AvailableStubs.size(); }

  Error redirectInner(JITDylib &TargetJD, const SymbolAddrMap &NewDests);
  Error grow(unsigned Need);

  ExecutionSession &ES;
  ObjectLinkingLayer &ObjLinkingLayer;
  JITDylib &JD;
  jitlink::AnonymousPointerCreator AnonymousPtrCreator;
  jitlink::PointerJumpStubCreator PtrJumpStubCreator;

  std::vector<StubHandle> AvailableStubs;
  using SymbolToStubMap = DenseMap<SymbolStringPtr, StubHandle>;
  DenseMap<JITDylib *, SymbolToStubMap> SymbolToStubs;
  std::vector<ExecutorSymbolDef> JumpStubs;
  std::vector<ExecutorSymbolDef> StubPointers;
  DenseMap<ResourceKey, std::vector<SymbolStringPtr>> TrackedResources;

  std::mutex Mutex;
};

class RewriteRedirectableSymbolManager : public RedirectableSymbolManager {
public:
  static Expected<std::unique_ptr<RedirectableSymbolManager>>
  Create(ExecutionSession &ES, ObjectLinkingLayer &ObjLinkingLayer,
         JITDylib &JD) {
    Error Err = Error::success();
    auto RM = std::unique_ptr<RedirectableSymbolManager>(
        new RewriteRedirectableSymbolManager(ES, ObjLinkingLayer, JD, Err));
    if (Err)
      return Err;
    auto RM2 = JITLinkRedirectableSymbolManager::Create(ES, ObjLinkingLayer, JD);
    if (!RM2)
      return RM2.takeError();
    ((RewriteRedirectableSymbolManager&)(*RM)).Base = std::move(*RM2);
    return std::move(RM);
  }

  void emitRedirectableSymbols(std::unique_ptr<MaterializationResponsibility> R,
                               const SymbolAddrMap &InitialDests) override;

  Error redirect(JITDylib &TargetJD, const SymbolAddrMap &NewDests) override;
private:
  RewriteRedirectableSymbolManager(ExecutionSession &ES,
                                   ObjectLinkingLayer &ObjLinkingLayer,
                                   JITDylib &JD, Error &Err)
      : ES(ES), ObjLinkingLayer(ObjLinkingLayer), JD(JD) {
    ObjLinkingLayer.addPlugin(std::make_unique<RewritePlugin>(*this));
    if (Err)
      return;
  }

  class RewritePlugin : public ObjectLinkingLayer::Plugin {
  public:
    RewritePlugin(RewriteRedirectableSymbolManager& Parent) : Parent(Parent) {}
    ~RewritePlugin() {}
    void modifyPassConfig(MaterializationResponsibility &MR,
                                  jitlink::LinkGraph &G,
                                  jitlink::PassConfiguration &Config) {
      Config.PreFixupPasses.push_back([&](jitlink::LinkGraph& G) {
        return Error::success();
      });
      Config.PostFixupPasses.push_back([&](jitlink::LinkGraph& G) {
        //G.dump(dbgs());
        for (auto* B : G.blocks()) {
          for (auto it = B->edges().begin(); it != B->edges().end(); ) {
            auto& E = *it;
            if (E.getKind() == jitlink::x86_64::EdgeKind_x86_64::BranchPCRel32) {
              auto SymName = Parent.ES.intern(E.getTarget().getName());
              if (Parent.Redirected.count(SymName)) {
                char *BlockWorkingMem = B->getAlreadyMutableContent().data();
                char *FixupPtr = BlockWorkingMem + E.getOffset();
                Parent.Addresses[SymName].push_back({B->getFixupAddress(E),E.getAddend()});
                auto FixupAddress = B->getFixupAddress(E);
                auto Target = Parent.Redirected[SymName];
                int64_t Value =
                    Target - (FixupAddress + 4) + E.getAddend();
                if (LLVM_LIKELY(isInt<32>(Value)))
                  *(support::little32_t *)FixupPtr = Value;
                else
                  assert(false);
                it = B->removeEdge(it);
                continue;
              }
            }
            ++it;
          }
        }
        return Error::success();
      });
    }

    // Deprecated. Don't use this in new code. There will be a proper mechanism
    // for capturing object buffers.
    void notifyMaterializing(MaterializationResponsibility &MR,
                                     jitlink::LinkGraph &G,
                                     jitlink::JITLinkContext &Ctx,
                                     MemoryBufferRef InputObject) {}

    void notifyLoaded(MaterializationResponsibility &MR) {}
    Error notifyEmitted(MaterializationResponsibility &MR) {
      return Error::success();
    }
    Error notifyFailed(MaterializationResponsibility &MR) { return Error::success(); }
    Error notifyRemovingResources(JITDylib &JD, ResourceKey K) { return Error::success(); }
    void notifyTransferringResources(JITDylib &JD, ResourceKey DstKey, ResourceKey SrcKey) { }

    RewriteRedirectableSymbolManager& Parent;                                            
  };

  friend class RewritePlugin;

  ExecutionSession &ES;
  ObjectLinkingLayer &ObjLinkingLayer;
  JITDylib &JD;

  std::unique_ptr<RedirectableSymbolManager> Base;

  DenseMap<SymbolStringPtr, std::vector<std::pair<ExecutorAddr,int64_t>>> Addresses;
  DenseMap<SymbolStringPtr, ExecutorAddr> Redirected;

  std::mutex Mutex;
};

} // namespace orc
} // namespace llvm

#endif
