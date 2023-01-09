//===--------- IncrementalParser.cpp - Incremental Compilation  -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the class which performs incremental code compilation.
//
//===----------------------------------------------------------------------===//

#include "IncrementalParser.h"

#include "clang/AST/DeclContextInternals.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/CodeGen/BackendUtil.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/FrontendTool/Utils.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorLexer.h"
#include "clang/Parse/Parser.h"
#include "clang/Sema/Sema.h"

#include "llvm/Option/ArgList.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBufferRef.h"
#include "llvm/Support/Timer.h"

#include <sstream>

namespace clang {

/// A custom action enabling the incremental processing functionality.
///
/// The usual \p FrontendAction expects one call to ExecuteAction and once it
/// sees a call to \p EndSourceFile it deletes some of the important objects
/// such as \p Preprocessor and \p Sema assuming no further input will come.
///
/// \p IncrementalAction ensures it keep its underlying action's objects alive
/// as long as the \p IncrementalParser needs them.
///
class IncrementalAction : public WrapperFrontendAction {
private:
  bool IsTerminating = false;

public:
  IncrementalAction(CompilerInstance &CI, llvm::LLVMContext &LLVMCtx,
                    llvm::Error &Err)
      : WrapperFrontendAction([&]() {
          llvm::ErrorAsOutParameter EAO(&Err);
          std::unique_ptr<FrontendAction> Act;
          switch (CI.getFrontendOpts().ProgramAction) {
          default:
            Err = llvm::createStringError(
                std::errc::state_not_recoverable,
                "Driver initialization failed. "
                "Incremental mode for action %d is not supported",
                CI.getFrontendOpts().ProgramAction);
            return Act;
          case frontend::ASTDump:
            [[fallthrough]];
          case frontend::ASTPrint:
            [[fallthrough]];
          case frontend::ParseSyntaxOnly:
            Act = CreateFrontendAction(CI);
            break;
          case frontend::PluginAction:
            [[fallthrough]];
          case frontend::EmitAssembly:
            [[fallthrough]];
          case frontend::EmitBC:
            [[fallthrough]];
          case frontend::EmitObj:
            [[fallthrough]];
          case frontend::PrintPreprocessedInput:
            [[fallthrough]];
          case frontend::EmitLLVMOnly:
            Act.reset(new EmitLLVMOnlyAction(&LLVMCtx));
            break;
          }
          return Act;
        }()) {}
  FrontendAction *getWrapped() const { return WrappedAction.get(); }
  TranslationUnitKind getTranslationUnitKind() override {
    return TU_Incremental;
  }
  void ExecuteAction() override {
    CompilerInstance &CI = getCompilerInstance();
    assert(CI.hasPreprocessor() && "No PP!");

    // FIXME: Move the truncation aspect of this into Sema, we delayed this till
    // here so the source manager would be initialized.
    if (hasCodeCompletionSupport() &&
        !CI.getFrontendOpts().CodeCompletionAt.FileName.empty())
      CI.createCodeCompletionConsumer();

    // Use a code completion consumer?
    CodeCompleteConsumer *CompletionConsumer = nullptr;
    if (CI.hasCodeCompletionConsumer())
      CompletionConsumer = &CI.getCodeCompletionConsumer();

    Preprocessor &PP = CI.getPreprocessor();
    PP.enableIncrementalProcessing();
    PP.EnterMainSourceFile();

    if (!CI.hasSema())
      CI.createSema(getTranslationUnitKind(), CompletionConsumer);
  }

  // Do not terminate after processing the input. This allows us to keep various
  // clang objects alive and to incrementally grow the current TU.
  void EndSourceFile() override {
    // The WrappedAction can be nullptr if we issued an error in the ctor.
    if (IsTerminating && getWrapped())
      WrapperFrontendAction::EndSourceFile();
  }

  void FinalizeAction() {
    assert(!IsTerminating && "Already finalized!");
    IsTerminating = true;
    EndSourceFile();
  }
};

IncrementalParser::IncrementalParser(std::unique_ptr<CompilerInstance> Instance,
                                     llvm::LLVMContext &LLVMCtx,
                                     llvm::Error &Err)
    : CI(std::move(Instance)) {
  llvm::ErrorAsOutParameter EAO(&Err);
  Act = std::make_unique<IncrementalAction>(*CI, LLVMCtx, Err);
  if (Err)
    return;
  CI->ExecuteAction(*Act);
  Consumer = &CI->getASTConsumer();
  P.reset(
      new Parser(CI->getPreprocessor(), CI->getSema(), /*SkipBodies=*/false));
  P->Initialize();
}

IncrementalParser::~IncrementalParser() {
  P.reset();
  Act->FinalizeAction();
}

llvm::Expected<PartialTranslationUnit &>
IncrementalParser::ParseOrWrapTopLevelDecl() {
  // Recover resources if we crash before exiting this method.
  Sema &S = CI->getSema();
  llvm::CrashRecoveryContextCleanupRegistrar<Sema> CleanupSema(&S);
  Sema::GlobalEagerInstantiationScope GlobalInstantiations(S, /*Enabled=*/true);
  Sema::LocalEagerInstantiationScope LocalInstantiations(S);

  PTUs.emplace_back(PartialTranslationUnit());
  PartialTranslationUnit &LastPTU = PTUs.back();
  // Add a new PTU.
  ASTContext &C = S.getASTContext();
  C.addTranslationUnitDecl();
  LastPTU.TUPart = C.getTranslationUnitDecl();

  // Skip previous eof due to last incremental input.
  if (P->getCurToken().is(tok::eof)) {
    P->ConsumeToken();
    // FIXME: Clang does not call ExitScope on finalizing the regular TU, we
    // might want to do that around HandleEndOfTranslationUnit.
    P->ExitScope();
    S.CurContext = nullptr;
    // Start a new PTU.
    P->EnterScope(Scope::DeclScope);
    S.ActOnTranslationUnitScope(P->getCurScope());
  }

  Parser::DeclGroupPtrTy ADecl;
  Sema::ModuleImportState ImportState;
  for (bool AtEOF = P->ParseFirstTopLevelDecl(ADecl, ImportState); !AtEOF;
       AtEOF = P->ParseTopLevelDecl(ADecl, ImportState)) {
    // If we got a null return and something *was* parsed, ignore it.  This
    // is due to a top-level semicolon, an action override, or a parse error
    // skipping something.
    if (ADecl && !Consumer->HandleTopLevelDecl(ADecl.get()))
      return llvm::make_error<llvm::StringError>("Parsing failed. "
                                                 "The consumer rejected a decl",
                                                 std::error_code());
  }

  DiagnosticsEngine &Diags = getCI()->getDiagnostics();
  if (Diags.hasErrorOccurred()) {
    PartialTranslationUnit MostRecentPTU = {C.getTranslationUnitDecl(),
                                            nullptr};
    CleanUpPTU(MostRecentPTU);

    Diags.Reset(/*soft=*/true);
    Diags.getClient()->clear();
    return llvm::make_error<llvm::StringError>("Parsing failed.",
                                               std::error_code());
  }

  // Process any TopLevelDecls generated by #pragma weak.
  for (Decl *D : S.WeakTopLevelDecls()) {
    DeclGroupRef DGR(D);
    Consumer->HandleTopLevelDecl(DGR);
  }

  LocalInstantiations.perform();
  GlobalInstantiations.perform();

  Consumer->HandleTranslationUnit(C);

  return LastPTU;
}

static CodeGenerator *getCodeGen(FrontendAction *Act) {
  IncrementalAction *IncrAct = static_cast<IncrementalAction *>(Act);
  FrontendAction *WrappedAct = IncrAct->getWrapped();
  if (!WrappedAct->hasIRSupport())
    return nullptr;
  return static_cast<CodeGenAction *>(WrappedAct)->getCodeGenerator();
}

static std::unique_ptr<llvm::MemoryBuffer>
CreateMemoryBuffer(llvm::StringRef SourceName, llvm::StringRef Str) {
  // Create an uninitialized memory buffer, copy code in and append "\n"
  size_t StrSize = Str.size(); // don't include trailing 0
  // MemBuffer size should *not* include terminating zero
  std::unique_ptr<llvm::MemoryBuffer> MB(
      llvm::WritableMemoryBuffer::getNewUninitMemBuffer(StrSize + 1,
                                                        SourceName));
  char *MBStart = const_cast<char *>(MB->getBufferStart());
  memcpy(MBStart, Str.data(), StrSize);
  MBStart[StrSize] = '\n';
  return MB;
}

class IncrementalInputReceiver : public SourceFileGrower {
public:
  IncrementalInputReceiver(Preprocessor &PP, SourceManager &SM, IncrementalParser::ReceiveAdditionalLine RecvLine): 
    PP(PP), SM(SM), RecvLine(RecvLine) {
  }

  llvm::Expected<FileID>
  Receive(StringRef InitialCode,
                                                StringRef SourceName) {
    // Buffer holding collected source code
    CodeBuffer = InitialCode.str();
    CodeBuffer += '\n';
    const clang::FileEntry* FE
      = SM.getFileManager().getVirtualFile(SourceName, CodeBuffer.size(),
                                           0 /* mod time*/);
    CurFileID = SM.createFileID(FE, SourceLocation(), SrcMgr::C_User);
    PP.setSourceFileGrower(this);
    SM.overrideFileContents(FE, llvm::MemoryBufferRef(CodeBuffer, ""));

    SourceLocation NewLoc = SM.getLocForStartOfFile(CurFileID);
    if (PP.EnterSourceFile(CurFileID, /*DirLookup=*/nullptr, NewLoc))
      return llvm::make_error<llvm::StringError>("Parsing failed. "
                                                 "Cannot enter source file.",
                                                 std::error_code());
    Token Tok;
    do {
      if (Err) {
        PP.EndSourceFile();
        PP.setSourceFileGrower(nullptr);
        return std::move(Err);
      }
      PP.Lex(Tok);
      switch(Tok.getKind()) {
        case tok::l_brace:
        case tok::l_paren:
        case tok::l_square: {
          BraceLevel[Tok.getKind()]++;
          break;
        }
        case tok::r_brace:
        case tok::r_paren:
        case tok::r_square: {
          if (BraceLevel[ToLBracket(Tok.getKind())] == 0) {
            PP.EndSourceFile();
            PP.setSourceFileGrower(nullptr);
            return llvm::make_error<llvm::StringError>("Parsing failed. "
                                                      "Unmathced braces.",
                                                      std::error_code());
          }
          BraceLevel[ToLBracket(Tok.getKind())]--;
          break;
        }
        default:
          break;
      }
    } while (Tok.isNot(tok::eof));
    PP.EndSourceFile();
    PP.setSourceFileGrower(nullptr);

    return SM.createFileID(CreateMemoryBuffer(SourceName, CodeBuffer),
                         SrcMgr::C_User, /*LoadedID=*/0,
                         /*LoadedOffset=*/0);
  }

  tok::TokenKind ToLBracket(tok::TokenKind Tok) {
    switch (Tok) {
      case tok::r_brace:
        return tok::l_brace;
      case tok::r_paren:
        return tok::l_paren;
      case tok::r_square:
        return tok::l_square;
      default:
        return tok::unknown;
    }
  }

  ~IncrementalInputReceiver() = default;

  bool TryGrowFile(FileID FileID) override {
    if (FileID != CurFileID)
      return false;
    if (IsBraceLevelZero())
      return false;
     if (auto Line = RecvLine()) {
        CodeBuffer += *Line;
        CodeBuffer += '\n';
        const FileEntry* Entry = SM.getFileEntryForID(CurFileID);
        assert(Entry);
        SM.overrideFileContents(Entry, llvm::MemoryBufferRef(CodeBuffer, ""));
        return true;
      } 
      Err = llvm::make_error<llvm::StringError>("Parsing failed. "
                                                  "Truncated code.",
                                                  std::error_code());
      return false;            
  }

  bool IsBraceLevelZero() {
    PreprocessorLexer* CurLexer = PP.getCurrentLexer();
    if (CurLexer && CurLexer->conditional_begin() != CurLexer->conditional_end())
      return false;
    for (auto& It : BraceLevel) {
      if (It.second)
        return false;
    }
    return true;
  }
private:
  Preprocessor &PP;
  SourceManager &SM;
  llvm::Error Err = llvm::Error::success();
  FileID CurFileID;
  std::string CodeBuffer;
  IncrementalParser::ReceiveAdditionalLine RecvLine;
  llvm::DenseMap<unsigned short, unsigned> BraceLevel;
};

// If RecvLine is not null, this will repeatedly call RecvLine function to fetch
// the additional lines required to finish a cut-off multiline function
// definition.
llvm::Expected<FileID>
IncrementalParser::ReceiveCompleteSourceInput(ReceiveAdditionalLine RecvLine,
                                              StringRef InitialCode,
                                              StringRef SourceName) {
  IncrementalInputReceiver InputReciever(CI->getPreprocessor(), CI->getSourceManager(), RecvLine);
  return InputReciever.Receive(InitialCode, SourceName);
}

llvm::Expected<PartialTranslationUnit &>
  IncrementalParser::Parse(llvm::StringRef Input,
                          ReceiveAdditionalLine RecvLine) {
  Preprocessor &PP = CI->getPreprocessor();
  assert(PP.isIncrementalProcessingEnabled() && "Not in incremental mode!?");

  auto SourceName = ("input_line_" + llvm::Twine(InputCount++)).str();

  SourceManager &SM = CI->getSourceManager();
  auto FID = ReceiveCompleteSourceInput(RecvLine, Input, SourceName);
  if (!FID)
    return FID.takeError();

  SourceLocation NewLoc = SM.getLocForStartOfFile(*FID);
  if (PP.EnterSourceFile(*FID, /*DirLookup=*/nullptr, NewLoc))
    return llvm::make_error<llvm::StringError>("Parsing failed. "
                                            "Cannot enter source file.",
                                              std::error_code());
  auto PTU = ParseOrWrapTopLevelDecl();
  if (!PTU)
    return PTU.takeError();

  if (PP.getLangOpts().DelayedTemplateParsing) {
    // Microsoft-specific:
    // Late parsed templates can leave unswallowed "macro"-like tokens.
    // They will seriously confuse the Parser when entering the next
    // source file. So lex until we are EOF.
    Token Tok;
    do {
      PP.Lex(Tok);
    } while (Tok.isNot(tok::eof));
  }

  Token AssertTok;
  PP.Lex(AssertTok);
  assert(AssertTok.is(tok::eof) &&
         "Lexer must be EOF when starting incremental parse!");

  if (CodeGenerator *CG = getCodeGen(Act.get())) {
    std::unique_ptr<llvm::Module> M(CG->ReleaseModule());
    CG->StartModule("incr_module_" + std::to_string(PTUs.size()),
                    M->getContext());

    PTU->TheModule = std::move(M);
  }

  return PTU;
}

void IncrementalParser::CleanUpPTU(PartialTranslationUnit &PTU) {
  TranslationUnitDecl *MostRecentTU = PTU.TUPart;
  TranslationUnitDecl *FirstTU = MostRecentTU->getFirstDecl();
  if (StoredDeclsMap *Map = FirstTU->getPrimaryContext()->getLookupPtr()) {
    for (auto I = Map->begin(); I != Map->end(); ++I) {
      StoredDeclsList &List = I->second;
      DeclContextLookupResult R = List.getLookupResult();
      for (NamedDecl *D : R) {
        if (D->getTranslationUnitDecl() == MostRecentTU) {
          List.remove(D);
        }
      }
      if (List.isNull())
        Map->erase(I);
    }
  }
}

llvm::StringRef IncrementalParser::GetMangledName(GlobalDecl GD) const {
  CodeGenerator *CG = getCodeGen(Act.get());
  assert(CG);
  return CG->GetMangledName(GD);
}

} // end namespace clang
