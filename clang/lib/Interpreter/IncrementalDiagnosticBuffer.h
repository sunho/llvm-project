//===-- IncrementalDiagnosticBuffer.h - Incremental Diagnostics --*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the class which collects diagnostics messages.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_INTERPRETER_INCREMENTALDIAGNOSTICBUFFER_H
#define LLVM_CLANG_LIB_INTERPRETER_INCREMENTALDIAGNOSTICBUFFER_H

#include "clang/Basic/DiagnosticLex.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "llvm/Support/Error.h"

namespace clang {

/// Collect diagnostics messages to buffer that can be flushed or
/// cleared based on the circumstances.
/// While collecting diagnostics messages, this looks whether
/// certain interesting diagnostic kinds have happend.
class IncrementalDiagnosticBuffer : public DiagnosticConsumer {
public:
  IncrementalDiagnosticBuffer(DiagnosticOptions &DiagOpts)
      : TextStream(TextBuffer),
        TextPrinter(
            std::make_unique<TextDiagnosticPrinter>(TextStream, &DiagOpts)) {}
  ~IncrementalDiagnosticBuffer() = default;

  /// Whether err_pp_unterminated_conditional has happend since the last
  /// flush/clear. If true, it indicates that #ifdef or #if directive was not
  /// finished off by #endif.
  bool HasTruncatedConditionalDirective() const {
    return TruncatedConditionalDirective;
  }

  /// Clear the collected diagnostic messages.
  void ClearDiagnostics() {
    TruncatedConditionalDirective = false;
    TextBuffer.clear();
  }

  /// Get the collected diagnostic messages as a string buffer.
  /// This also clears the collected diagnostic message buffer.
  std::string FlushDiagnostics() {
    std::string Result = std::move(TextBuffer);
    ClearDiagnostics();
    return Result;
  }

  void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                        const Diagnostic &Info) override {
    switch (Info.getID()) {
    case diag::err_pp_unterminated_conditional: {
      TruncatedConditionalDirective = true;
      break;
    }
    default:
      break;
    }
    TextPrinter->HandleDiagnostic(DiagLevel, Info);
  }

  void BeginSourceFile(const clang::LangOptions &LO,
                       const clang::Preprocessor *PP) override {
    TextPrinter->BeginSourceFile(LO, PP);
  }

  void EndSourceFile() override { TextPrinter->EndSourceFile(); }

private:
  bool TruncatedConditionalDirective = false;
  std::string TextBuffer;
  llvm::raw_string_ostream TextStream;
  std::unique_ptr<TextDiagnosticPrinter> TextPrinter;
};

} // namespace clang

#endif