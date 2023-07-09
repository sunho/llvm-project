#include "OrcTestCommon.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/JITLink/JITLinkMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/Shared/ExecutorAddress.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace llvm::orc;
using namespace llvm::jitlink;

static int initialTarget() { return 42; }
static int middleTarget() { return 13; }
static int finalTarget() { return 53; }

class JITLinkRedirectionManagerTest : public testing::Test {
public:
  ~JITLinkRedirectionManagerTest() {
    if (auto Err = ES.endSession())
      ES.reportError(std::move(Err));
  }

protected:
  ExecutionSession ES{std::make_unique<UnsupportedExecutorProcessControl>()};
  JITDylib &JD = ES.createBareJITDylib("main");
  ObjectLinkingLayer ObjLinkingLayer{
      ES, std::make_unique<InProcessMemoryManager>(4096)};
};

TEST_F(JITLinkRedirectionManagerTest, BasicRedirectionOperation) {
  auto JTMB = JITTargetMachineBuilder::detectHost();

  // Bail out if we can not detect the host.
  if (!JTMB) {
    consumeError(JTMB.takeError());
    GTEST_SKIP();
  }

  // auto RM = JITLinkRedirectionManager::Create(JTMB->getTargetTriple(), ES, ObjLinkingLayer);
  // // Bail out if we can not create 
  // if (!RM) {
  //   consumeError(RM.takeError());
  //   GTEST_SKIP();
  // }

  // auto DefineTarget = [&](StringRef TargetName, ExecutorAddr Addr) {
  //   auto Target = ES.intern(TargetName);
  //   cantFail(JD.define(std::make_unique<SimpleMaterializationUnit>(
  //       SymbolFlagsMap({{Target, JITSymbolFlags::Exported}}),
  //       [&](std::unique_ptr<MaterializationResponsibility> R) {
  //         // No dependencies registered, can't fail.
  //         cantFail(R->notifyResolved({{Target,
  //                                     {Addr,
  //                                       JITSymbolFlags::Exported}}}));
  //         cantFail(R->notifyEmitted());
  //       })));
  //   return cantFail(ES.lookup({&JD}, TargetName));
  // };

  // auto InitialTarget = DefineTarget("InitialTarget", ExecutorAddr::fromPtr(&initialTarget));
  // auto MiddleTarget = DefineTarget("MiddleTarget", ExecutorAddr::fromPtr(&middleTarget));
  // auto FinalTarget = DefineTarget("FinalTarget", ExecutorAddr::fromPtr(&finalTarget));

  // auto RedirectableSymbol = ES.intern("RedirectableTarget");
  // cantFail((*RM)->createRedirectableSymbols(JD, {RedirectableSymbol, InitialTarget}));
  // auto RTDef = cantFail(ES.lookup({&JD}, RedirectableSymbol));

  // auto RTPtr = RTDef.getAddress().toPtr<int (*)()>();
  // auto Result = RTPtr();
  // EXPECT_EQ(Result, 42) << "Failed to call initial target";
  
  // cantFail((*RM)->redirect(JD, {RedirectableSymbol, MiddleTarget}));
  // Result = RTPtr();
  // EXPECT_EQ(Result, 13) << "Failed to call middle redirected target";

  // cantFail((*RM)->redirect(JD, {RedirectableSymbol, FinalTarget}));
  // Result = RTPtr();
  // EXPECT_EQ(Result, 53) << "Failed to call redirected target";
}
