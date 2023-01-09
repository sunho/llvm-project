//===- Lexer.cpp - C Language Family Lexer --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file implements the Lexer and Token interfaces.
//
//===----------------------------------------------------------------------===//

#include "clang/Lex/Lexer.h"
#include "UnicodeCharSets.h"
#include "clang/Basic/CharInfo.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/LexDiagnostic.h"
#include "clang/Lex/LiteralSupport.h"
#include "clang/Lex/MultipleIncludeOpt.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Lex/Token.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ConvertUTF.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/MemoryBufferRef.h"
#include "llvm/Support/NativeFormatting.h"
#include "llvm/Support/Unicode.h"
#include "llvm/Support/UnicodeCharRanges.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <tuple>
#include <utility>

using namespace clang;

//===----------------------------------------------------------------------===//
// Token Class Implementation
//===----------------------------------------------------------------------===//

/// isObjCAtKeyword - Return true if we have an ObjC keyword identifier.
bool Token::isObjCAtKeyword(tok::ObjCKeywordKind objcKey) const {
  if (isAnnotation())
    return false;
  if (IdentifierInfo *II = getIdentifierInfo())
    return II->getObjCKeywordID() == objcKey;
  return false;
}

/// getObjCKeywordID - Return the ObjC keyword kind.
tok::ObjCKeywordKind Token::getObjCKeywordID() const {
  if (isAnnotation())
    return tok::objc_not_keyword;
  IdentifierInfo *specId = getIdentifierInfo();
  return specId ? specId->getObjCKeywordID() : tok::objc_not_keyword;
}

//===----------------------------------------------------------------------===//
// Lexer Class Implementation
//===----------------------------------------------------------------------===//

void Lexer::anchor() {}

void Lexer::InitLexer(const char *BufStart, unsigned BufOffset,
                      unsigned BufSize) {
  BufferStart = BufStart;
  BufferOffset = BufOffset;
  BufferSize = BufSize;

  assert(BufStart[BufSize] == 0 &&
         "We assume that the input buffer has a null character at the end"
         " to simplify lexing!");

  // Check whether we have a BOM in the beginning of the buffer. If yes - act
  // accordingly. Right now we support only UTF-8 with and without BOM, so, just
  // skip the UTF-8 BOM if it's present.
  if (BufferOffset == 0) {
    // Determine the size of the BOM.
    StringRef Buf(BufferStart, BufferSize);
    size_t BOMLength = llvm::StringSwitch<size_t>(Buf)
      .StartsWith("\xEF\xBB\xBF", 3) // UTF-8 BOM
      .Default(0);

    // Skip the BOM.
    BufferOffset += BOMLength;
  }

  Is_PragmaLexer = false;
  CurrentConflictMarkerState = CMK_None;

  // Start of the file is a start of line.
  IsAtStartOfLine = true;
  IsAtPhysicalStartOfLine = true;

  HasLeadingSpace = false;
  HasLeadingEmptyMacro = false;

  // We are not after parsing a #.
  ParsingPreprocessorDirective = false;

  // We are not after parsing #include.
  ParsingFilename = false;

  // We are not in raw mode.  Raw mode disables diagnostics and interpretation
  // of tokens (e.g. identifiers, thus disabling macro expansion).  It is used
  // to quickly lex the tokens of the buffer, e.g. when handling a "#if 0" block
  // or otherwise skipping over tokens.
  LexingRawMode = false;

  // Default to not keeping comments.
  ExtendedTokenMode = 0;

  NewLineOffset = llvm::None;
}

/// Lexer constructor - Create a new lexer object for the specified buffer
/// with the specified preprocessor managing the lexing process.  This lexer
/// assumes that the associated file buffer and Preprocessor objects will
/// outlive it, so it doesn't take ownership of either of them.
Lexer::Lexer(FileID FID, const llvm::MemoryBufferRef &InputFile,
             Preprocessor &PP, bool IsFirstIncludeOfFile, GrowBufferCallback GrowBuffer)
    : PreprocessorLexer(&PP, FID),
      FileLoc(PP.getSourceManager().getLocForStartOfFile(FID)),
      LangOpts(PP.getLangOpts()), LineComment(LangOpts.LineComment),
      IsFirstTimeLexingFile(IsFirstIncludeOfFile), 
      GrowBuffer(std::move(GrowBuffer)) {
  InitLexer(InputFile.getBufferStart(), 0,
            InputFile.getBufferSize());

  resetExtendedTokenMode();
}

/// Lexer constructor - Create a new raw lexer object.  This object is only
/// suitable for calls to 'LexFromRawLexer'.  This lexer assumes that the text
/// range will outlive it, so it doesn't take ownership of it.
Lexer::Lexer(SourceLocation fileloc, const LangOptions &langOpts,
             const char *BufStart, const char *BufPtr, const char *BufEnd,
             bool IsFirstIncludeOfFile)
    : FileLoc(fileloc), LangOpts(langOpts), LineComment(LangOpts.LineComment),
      IsFirstTimeLexingFile(IsFirstIncludeOfFile) {
  InitLexer(BufStart, BufPtr-BufStart, BufEnd-BufStart);
  // We *are* in raw mode.
  LexingRawMode = true;
}

/// Lexer constructor - Create a new raw lexer object.  This object is only
/// suitable for calls to 'LexFromRawLexer'.  This lexer assumes that the text
/// range will outlive it, so it doesn't take ownership of it.
Lexer::Lexer(FileID FID, const llvm::MemoryBufferRef &FromFile,
             const SourceManager &SM, const LangOptions &langOpts,
             bool IsFirstIncludeOfFile)
    : Lexer(SM.getLocForStartOfFile(FID), langOpts, FromFile.getBufferStart(),
            FromFile.getBufferStart(), FromFile.getBufferEnd(),
            IsFirstIncludeOfFile) {}

void Lexer::resetExtendedTokenMode() {
  assert(PP && "Cannot reset token mode without a preprocessor");
  if (LangOpts.TraditionalCPP)
    SetKeepWhitespaceMode(true);
  else
    SetCommentRetentionState(PP->getCommentRetentionState());
}

/// Create_PragmaLexer: Lexer constructor - Create a new lexer object for
/// _Pragma expansion.  This has a variety of magic semantics that this method
/// sets up.  It returns a new'd Lexer that must be delete'd when done.
///
/// On entrance to this routine, TokStartLoc is a macro location which has a
/// spelling loc that indicates the bytes to be lexed for the token and an
/// expansion location that indicates where all lexed tokens should be
/// "expanded from".
///
/// TODO: It would really be nice to make _Pragma just be a wrapper around a
/// normal lexer that remaps tokens as they fly by.  This would require making
/// Preprocessor::Lex virtual.  Given that, we could just dump in a magic lexer
/// interface that could handle this stuff.  This would pull GetMappedTokenLoc
/// out of the critical path of the lexer!
///
Lexer *Lexer::Create_PragmaLexer(SourceLocation SpellingLoc,
                                 SourceLocation ExpansionLocStart,
                                 SourceLocation ExpansionLocEnd,
                                 unsigned TokLen, Preprocessor &PP) {
  SourceManager &SM = PP.getSourceManager();

  // Create the lexer as if we were going to lex the file normally.
  FileID SpellingFID = SM.getFileID(SpellingLoc);
  llvm::MemoryBufferRef InputFile = SM.getBufferOrFake(SpellingFID);
  Lexer *L = new Lexer(SpellingFID, InputFile, PP);

  // Now that the lexer is created, change the start/end locations so that we
  // just lex the subsection of the file that we want.  This is lexing from a
  // scratch buffer.
  const char *StrData = SM.getCharacterData(SpellingLoc);

  L->BufferStart = InputFile.getBufferStart();
  L->BufferOffset = StrData - InputFile.getBufferStart(); // FIXME: this is wrong
  L->BufferSize = L->BufferOffset + TokLen;
  assert(L->BufferStart[L->BufferSize] == 0 && "Buffer is not nul terminated!");

  // Set the SourceLocation with the remapping information.  This ensures that
  // GetMappedTokenLoc will remap the tokens as they are lexed.
  L->FileLoc = SM.createExpansionLoc(SM.getLocForStartOfFile(SpellingFID),
                                     ExpansionLocStart,
                                     ExpansionLocEnd, TokLen);

  // Ensure that the lexer thinks it is inside a directive, so that end \n will
  // return an EOD token.
  L->ParsingPreprocessorDirective = true;

  // This lexer really is for _Pragma.
  L->Is_PragmaLexer = true;
  return L;
}

void Lexer::seek(unsigned Offset, bool IsAtStartOfLine) {
  this->IsAtPhysicalStartOfLine = IsAtStartOfLine;
  this->IsAtStartOfLine = IsAtStartOfLine;
  assert(Offset <= BufferSize);
  BufferOffset = Offset;
}

template <typename T> static void StringifyImpl(T &Str, char Quote) {
  typename T::size_type i = 0, e = Str.size();
  while (i < e) {
    if (Str[i] == '\\' || Str[i] == Quote) {
      Str.insert(Str.begin() + i, '\\');
      i += 2;
      ++e;
    } else if (Str[i] == '\n' || Str[i] == '\r') {
      // Replace '\r\n' and '\n\r' to '\\' followed by 'n'.
      if ((i < e - 1) && (Str[i + 1] == '\n' || Str[i + 1] == '\r') &&
          Str[i] != Str[i + 1]) {
        Str[i] = '\\';
        Str[i + 1] = 'n';
      } else {
        // Replace '\n' and '\r' to '\\' followed by 'n'.
        Str[i] = '\\';
        Str.insert(Str.begin() + i + 1, 'n');
        ++e;
      }
      i += 2;
    } else
      ++i;
  }
}

std::string Lexer::Stringify(StringRef Str, bool Charify) {
  std::string Result = std::string(Str);
  char Quote = Charify ? '\'' : '"';
  StringifyImpl(Result, Quote);
  return Result;
}

void Lexer::Stringify(SmallVectorImpl<char> &Str) { StringifyImpl(Str, '"'); }

//===----------------------------------------------------------------------===//
// Token Spelling
//===----------------------------------------------------------------------===//

/// Slow case of getSpelling. Extract the characters comprising the
/// spelling of this token from the provided input buffer.
static size_t getSpellingSlow(const Token &Tok, const char *BufPtr,
                              const LangOptions &LangOpts, char *Spelling) {
  assert(Tok.needsCleaning() && "getSpellingSlow called on simple token");

  size_t Length = 0;
  const char *BufEnd = BufPtr + Tok.getLength();

  if (tok::isStringLiteral(Tok.getKind())) {
    // Munch the encoding-prefix and opening double-quote.
    while (BufPtr < BufEnd) {
      unsigned Size;
      Spelling[Length++] = Lexer::getCharAndSizeNoWarn(BufPtr, Size, LangOpts);
      BufPtr += Size;

      if (Spelling[Length - 1] == '"')
        break;
    }

    // Raw string literals need special handling; trigraph expansion and line
    // splicing do not occur within their d-char-sequence nor within their
    // r-char-sequence.
    if (Length >= 2 &&
        Spelling[Length - 2] == 'R' && Spelling[Length - 1] == '"') {
      // Search backwards from the end of the token to find the matching closing
      // quote.
      const char *RawEnd = BufEnd;
      do --RawEnd; while (*RawEnd != '"');
      size_t RawLength = RawEnd - BufPtr + 1;

      // Everything between the quotes is included verbatim in the spelling.
      memcpy(Spelling + Length, BufPtr, RawLength);
      Length += RawLength;
      BufPtr += RawLength;

      // The rest of the token is lexed normally.
    }
  }

  while (BufPtr < BufEnd) {
    unsigned Size;
    Spelling[Length++] = Lexer::getCharAndSizeNoWarn(BufPtr, Size, LangOpts);
    BufPtr += Size;
  }

  assert(Length < Tok.getLength() &&
         "NeedsCleaning flag set on token that didn't need cleaning!");
  return Length;
}

/// getSpelling() - Return the 'spelling' of this token.  The spelling of a
/// token are the characters used to represent the token in the source file
/// after trigraph expansion and escaped-newline folding.  In particular, this
/// wants to get the true, uncanonicalized, spelling of things like digraphs
/// UCNs, etc.
StringRef Lexer::getSpelling(SourceLocation loc,
                             SmallVectorImpl<char> &buffer,
                             const SourceManager &SM,
                             const LangOptions &options,
                             bool *invalid) {
  // Break down the source location.
  std::pair<FileID, unsigned> locInfo = SM.getDecomposedLoc(loc);

  // Try to the load the file buffer.
  bool invalidTemp = false;
  StringRef file = SM.getBufferData(locInfo.first, &invalidTemp);
  if (invalidTemp) {
    if (invalid) *invalid = true;
    return {};
  }

  const char *tokenBegin = file.data() + locInfo.second;

  // Lex from the start of the given location.
  Lexer lexer(SM.getLocForStartOfFile(locInfo.first), options,
              file.begin(), tokenBegin, file.end());
  Token token;
  lexer.LexFromRawLexer(token);

  unsigned length = token.getLength();

  // Common case:  no need for cleaning.
  if (!token.needsCleaning())
    return StringRef(tokenBegin, length);

  // Hard case, we need to relex the characters into the string.
  buffer.resize(length);
  buffer.resize(getSpellingSlow(token, tokenBegin, options, buffer.data()));
  return StringRef(buffer.data(), buffer.size());
}

/// getSpelling() - Return the 'spelling' of this token.  The spelling of a
/// token are the characters used to represent the token in the source file
/// after trigraph expansion and escaped-newline folding.  In particular, this
/// wants to get the true, uncanonicalized, spelling of things like digraphs
/// UCNs, etc.
std::string Lexer::getSpelling(const Token &Tok, const SourceManager &SourceMgr,
                               const LangOptions &LangOpts, bool *Invalid) {
  assert((int)Tok.getLength() >= 0 && "Token character range is bogus!");

  bool CharDataInvalid = false;
  const char *TokStart = SourceMgr.getCharacterData(Tok.getLocation(),
                                                    &CharDataInvalid);
  if (Invalid)
    *Invalid = CharDataInvalid;
  if (CharDataInvalid)
    return {};

  // If this token contains nothing interesting, return it directly.
  if (!Tok.needsCleaning())
    return std::string(TokStart, TokStart + Tok.getLength());

  std::string Result;
  Result.resize(Tok.getLength());
  Result.resize(getSpellingSlow(Tok, TokStart, LangOpts, &*Result.begin()));
  return Result;
}

/// getSpelling - This method is used to get the spelling of a token into a
/// preallocated buffer, instead of as an std::string.  The caller is required
/// to allocate enough space for the token, which is guaranteed to be at least
/// Tok.getLength() bytes long.  The actual length of the token is returned.
///
/// Note that this method may do two possible things: it may either fill in
/// the buffer specified with characters, or it may *change the input pointer*
/// to point to a constant buffer with the data already in it (avoiding a
/// copy).  The caller is not allowed to modify the returned buffer pointer
/// if an internal buffer is returned.
unsigned Lexer::getSpelling(const Token &Tok, const char *&Buffer,
                            const SourceManager &SourceMgr,
                            const LangOptions &LangOpts, bool *Invalid) {
  assert((int)Tok.getLength() >= 0 && "Token character range is bogus!");

  const char *TokStart = nullptr;
  // NOTE: this has to be checked *before* testing for an IdentifierInfo.
  if (Tok.is(tok::raw_identifier))
    TokStart = Tok.getRawIdentifier().data();
  else if (!Tok.hasUCN()) {
    if (const IdentifierInfo *II = Tok.getIdentifierInfo()) {
      // Just return the string from the identifier table, which is very quick.
      Buffer = II->getNameStart();
      return II->getLength();
    }
  }

  // NOTE: this can be checked even after testing for an IdentifierInfo.
  if (Tok.isLiteral())
    TokStart = Tok.getLiteralData();

  if (!TokStart) {
    // Compute the start of the token in the input lexer buffer.
    bool CharDataInvalid = false;
    TokStart = SourceMgr.getCharacterData(Tok.getLocation(), &CharDataInvalid);
    if (Invalid)
      *Invalid = CharDataInvalid;
    if (CharDataInvalid) {
      Buffer = "";
      return 0;
    }
  }

  // If this token contains nothing interesting, return it directly.
  if (!Tok.needsCleaning()) {
    Buffer = TokStart;
    return Tok.getLength();
  }

  // Otherwise, hard case, relex the characters into the string.
  return getSpellingSlow(Tok, TokStart, LangOpts, const_cast<char*>(Buffer));
}

 bool Lexer::TryExpandBuffer() {
  if (!GrowBuffer)
    return false;
  
  auto NewBuffer = GrowBuffer();
  if (!NewBuffer)
    return false;

  BufferStart = NewBuffer->getBufferStart();
  BufferSize = NewBuffer->getBufferSize();
  return true;
}

/// MeasureTokenLength - Relex the token at the specified location and return
/// its length in bytes in the input file.  If the token needs cleaning (e.g.
/// includes a trigraph or an escaped newline) then this count includes bytes
/// that are part of that.
unsigned Lexer::MeasureTokenLength(SourceLocation Loc,
                                   const SourceManager &SM,
                                   const LangOptions &LangOpts) {
  Token TheTok;
  if (getRawToken(Loc, TheTok, SM, LangOpts))
    return 0;
  return TheTok.getLength();
}

/// Relex the token at the specified location.
/// \returns true if there was a failure, false on success.
bool Lexer::getRawToken(SourceLocation Loc, Token &Result,
                        const SourceManager &SM,
                        const LangOptions &LangOpts,
                        bool IgnoreWhiteSpace) {
  // TODO: this could be special cased for common tokens like identifiers, ')',
  // etc to make this faster, if it mattered.  Just look at StrData[0] to handle
  // all obviously single-char tokens.  This could use
  // Lexer::isObviouslySimpleCharacter for example to handle identifiers or
  // something.

  // If this comes from a macro expansion, we really do want the macro name, not
  // the token this macro expanded to.
  Loc = SM.getExpansionLoc(Loc);
  std::pair<FileID, unsigned> LocInfo = SM.getDecomposedLoc(Loc);
  bool Invalid = false;
  StringRef Buffer = SM.getBufferData(LocInfo.first, &Invalid);
  if (Invalid)
    return true;

  const char *StrData = Buffer.data()+LocInfo.second;

  if (!IgnoreWhiteSpace && isWhitespace(StrData[0]))
    return true;

  // Create a lexer starting at the beginning of this token.
  Lexer TheLexer(SM.getLocForStartOfFile(LocInfo.first), LangOpts,
                 Buffer.begin(), StrData, Buffer.end());
  TheLexer.SetCommentRetentionState(true);
  TheLexer.LexFromRawLexer(Result);
  return false;
}

/// Returns the pointer that points to the beginning of line that contains
/// the given offset, or null if the offset if invalid.
static const char *findBeginningOfLine(StringRef Buffer, unsigned Offset) {
  const char *BufStart = Buffer.data();
  if (Offset >= Buffer.size())
    return nullptr;

  const char *LexStart = BufStart + Offset;
  for (; LexStart != BufStart; --LexStart) {
    if (isVerticalWhitespace(LexStart[0]) &&
        !Lexer::isNewLineEscaped(BufStart, LexStart)) {
      // LexStart should point at first character of logical line.
      ++LexStart;
      break;
    }
  }
  return LexStart;
}

static SourceLocation getBeginningOfFileToken(SourceLocation Loc,
                                              const SourceManager &SM,
                                              const LangOptions &LangOpts) {
  assert(Loc.isFileID());
  std::pair<FileID, unsigned> LocInfo = SM.getDecomposedLoc(Loc);
  if (LocInfo.first.isInvalid())
    return Loc;

  bool Invalid = false;
  StringRef Buffer = SM.getBufferData(LocInfo.first, &Invalid);
  if (Invalid)
    return Loc;

  // Back up from the current location until we hit the beginning of a line
  // (or the buffer). We'll relex from that point.
  const char *StrData = Buffer.data() + LocInfo.second;
  const char *LexStart = findBeginningOfLine(Buffer, LocInfo.second);
  if (!LexStart || LexStart == StrData)
    return Loc;

  // Create a lexer starting at the beginning of this token.
  SourceLocation LexerStartLoc = Loc.getLocWithOffset(-LocInfo.second);
  Lexer TheLexer(LexerStartLoc, LangOpts, Buffer.data(), LexStart,
                 Buffer.end());
  TheLexer.SetCommentRetentionState(true);

  // Lex tokens until we find the token that contains the source location.
  Token TheTok;
  do {
    TheLexer.LexFromRawLexer(TheTok);

    if (TheLexer.getBufferLocation() > StrData) {
      // Lexing this token has taken the lexer past the source location we're
      // looking for. If the current token encompasses our source location,
      // return the beginning of that token.
      if (TheLexer.getBufferLocation() - TheTok.getLength() <= StrData)
        return TheTok.getLocation();

      // We ended up skipping over the source location entirely, which means
      // that it points into whitespace. We're done here.
      break;
    }
  } while (TheTok.getKind() != tok::eof);

  // We've passed our source location; just return the original source location.
  return Loc;
}

SourceLocation Lexer::GetBeginningOfToken(SourceLocation Loc,
                                          const SourceManager &SM,
                                          const LangOptions &LangOpts) {
  if (Loc.isFileID())
    return getBeginningOfFileToken(Loc, SM, LangOpts);

  if (!SM.isMacroArgExpansion(Loc))
    return Loc;

  SourceLocation FileLoc = SM.getSpellingLoc(Loc);
  SourceLocation BeginFileLoc = getBeginningOfFileToken(FileLoc, SM, LangOpts);
  std::pair<FileID, unsigned> FileLocInfo = SM.getDecomposedLoc(FileLoc);
  std::pair<FileID, unsigned> BeginFileLocInfo =
      SM.getDecomposedLoc(BeginFileLoc);
  assert(FileLocInfo.first == BeginFileLocInfo.first &&
         FileLocInfo.second >= BeginFileLocInfo.second);
  return Loc.getLocWithOffset(BeginFileLocInfo.second - FileLocInfo.second);
}

namespace {

enum PreambleDirectiveKind {
  PDK_Skipped,
  PDK_Unknown
};

} // namespace

PreambleBounds Lexer::ComputePreamble(StringRef Buffer,
                                      const LangOptions &LangOpts,
                                      unsigned MaxLines) {
  // Create a lexer starting at the beginning of the file. Note that we use a
  // "fake" file source location at offset 1 so that the lexer will track our
  // position within the file.
  const SourceLocation::UIntTy StartOffset = 1;
  SourceLocation FileLoc = SourceLocation::getFromRawEncoding(StartOffset);
  Lexer TheLexer(FileLoc, LangOpts, Buffer.begin(), Buffer.begin(),
                 Buffer.end());
  TheLexer.SetCommentRetentionState(true);

  bool InPreprocessorDirective = false;
  Token TheTok;
  SourceLocation ActiveCommentLoc;

  unsigned MaxLineOffset = 0;
  if (MaxLines) {
    const char* CurPtr = Buffer.begin();
    unsigned CurLine = 0;
    while (CurPtr != Buffer.end()) {
      char ch = *CurPtr++;
      if (ch == '\n') {
        ++CurLine;
        if (CurLine == MaxLines)
          break;
      }
    }
    if (CurPtr != Buffer.end())
      MaxLineOffset = CurPtr - Buffer.begin();
  }

  do {
    TheLexer.LexFromRawLexer(TheTok);

    if (InPreprocessorDirective) {
      // If we've hit the end of the file, we're done.
      if (TheTok.getKind() == tok::eof) {
        break;
      }

      // If we haven't hit the end of the preprocessor directive, skip this
      // token.
      if (!TheTok.isAtStartOfLine())
        continue;

      // We've passed the end of the preprocessor directive, and will look
      // at this token again below.
      InPreprocessorDirective = false;
    }

    // Keep track of the # of lines in the preamble.
    if (TheTok.isAtStartOfLine()) {
      unsigned TokOffset = TheTok.getLocation().getRawEncoding() - StartOffset;

      // If we were asked to limit the number of lines in the preamble,
      // and we're about to exceed that limit, we're done.
      if (MaxLineOffset && TokOffset >= MaxLineOffset)
        break;
    }

    // Comments are okay; skip over them.
    if (TheTok.getKind() == tok::comment) {
      if (ActiveCommentLoc.isInvalid())
        ActiveCommentLoc = TheTok.getLocation();
      continue;
    }

    if (TheTok.isAtStartOfLine() && TheTok.getKind() == tok::hash) {
      // This is the start of a preprocessor directive.
      Token HashTok = TheTok;
      InPreprocessorDirective = true;
      ActiveCommentLoc = SourceLocation();

      // Figure out which directive this is. Since we're lexing raw tokens,
      // we don't have an identifier table available. Instead, just look at
      // the raw identifier to recognize and categorize preprocessor directives.
      TheLexer.LexFromRawLexer(TheTok);
      if (TheTok.getKind() == tok::raw_identifier && !TheTok.needsCleaning()) {
        StringRef Keyword = TheTok.getRawIdentifier();
        PreambleDirectiveKind PDK
          = llvm::StringSwitch<PreambleDirectiveKind>(Keyword)
              .Case("include", PDK_Skipped)
              .Case("__include_macros", PDK_Skipped)
              .Case("define", PDK_Skipped)
              .Case("undef", PDK_Skipped)
              .Case("line", PDK_Skipped)
              .Case("error", PDK_Skipped)
              .Case("pragma", PDK_Skipped)
              .Case("import", PDK_Skipped)
              .Case("include_next", PDK_Skipped)
              .Case("warning", PDK_Skipped)
              .Case("ident", PDK_Skipped)
              .Case("sccs", PDK_Skipped)
              .Case("assert", PDK_Skipped)
              .Case("unassert", PDK_Skipped)
              .Case("if", PDK_Skipped)
              .Case("ifdef", PDK_Skipped)
              .Case("ifndef", PDK_Skipped)
              .Case("elif", PDK_Skipped)
              .Case("elifdef", PDK_Skipped)
              .Case("elifndef", PDK_Skipped)
              .Case("else", PDK_Skipped)
              .Case("endif", PDK_Skipped)
              .Default(PDK_Unknown);

        switch (PDK) {
        case PDK_Skipped:
          continue;

        case PDK_Unknown:
          // We don't know what this directive is; stop at the '#'.
          break;
        }
      }

      // We only end up here if we didn't recognize the preprocessor
      // directive or it was one that can't occur in the preamble at this
      // point. Roll back the current token to the location of the '#'.
      TheTok = HashTok;
    }

    // We hit a token that we don't recognize as being in the
    // "preprocessing only" part of the file, so we're no longer in
    // the preamble.
    break;
  } while (true);

  SourceLocation End;
  if (ActiveCommentLoc.isValid())
    End = ActiveCommentLoc; // don't truncate a decl comment.
  else
    End = TheTok.getLocation();

  return PreambleBounds(End.getRawEncoding() - FileLoc.getRawEncoding(),
                        TheTok.isAtStartOfLine());
}

unsigned Lexer::getTokenPrefixLength(SourceLocation TokStart, unsigned CharNo,
                                     const SourceManager &SM,
                                     const LangOptions &LangOpts) {
  // Figure out how many physical characters away the specified expansion
  // character is.  This needs to take into consideration newlines and
  // trigraphs.
  bool Invalid = false;
  const char *TokPtr = SM.getCharacterData(TokStart, &Invalid);

  // If they request the first char of the token, we're trivially done.
  if (Invalid || (CharNo == 0 && Lexer::isObviouslySimpleCharacter(*TokPtr)))
    return 0;

  unsigned PhysOffset = 0;

  // The usual case is that tokens don't contain anything interesting.  Skip
  // over the uninteresting characters.  If a token only consists of simple
  // chars, this method is extremely fast.
  while (Lexer::isObviouslySimpleCharacter(*TokPtr)) {
    if (CharNo == 0)
      return PhysOffset;
    ++TokPtr;
    --CharNo;
    ++PhysOffset;
  }

  // If we have a character that may be a trigraph or escaped newline, use a
  // lexer to parse it correctly.
  for (; CharNo; --CharNo) {
    unsigned Size;
    Lexer::getCharAndSizeNoWarn(TokPtr, Size, LangOpts);
    TokPtr += Size;
    PhysOffset += Size;
  }

  // Final detail: if we end up on an escaped newline, we want to return the
  // location of the actual byte of the token.  For example foo\<newline>bar
  // advanced by 3 should return the location of b, not of \\.  One compounding
  // detail of this is that the escape may be made by a trigraph.
  if (!Lexer::isObviouslySimpleCharacter(*TokPtr))
    PhysOffset += Lexer::SkipEscapedNewLines(TokPtr)-TokPtr;

  return PhysOffset;
}

/// Computes the source location just past the end of the
/// token at this source location.
///
/// This routine can be used to produce a source location that
/// points just past the end of the token referenced by \p Loc, and
/// is generally used when a diagnostic needs to point just after a
/// token where it expected something different that it received. If
/// the returned source location would not be meaningful (e.g., if
/// it points into a macro), this routine returns an invalid
/// source location.
///
/// \param Offset an offset from the end of the token, where the source
/// location should refer to. The default offset (0) produces a source
/// location pointing just past the end of the token; an offset of 1 produces
/// a source location pointing to the last character in the token, etc.
SourceLocation Lexer::getLocForEndOfToken(SourceLocation Loc, unsigned Offset,
                                          const SourceManager &SM,
                                          const LangOptions &LangOpts) {
  if (Loc.isInvalid())
    return {};

  if (Loc.isMacroID()) {
    if (Offset > 0 || !isAtEndOfMacroExpansion(Loc, SM, LangOpts, &Loc))
      return {}; // Points inside the macro expansion.
  }

  unsigned Len = Lexer::MeasureTokenLength(Loc, SM, LangOpts);
  if (Len > Offset)
    Len = Len - Offset;
  else
    return Loc;

  return Loc.getLocWithOffset(Len);
}

/// Returns true if the given MacroID location points at the first
/// token of the macro expansion.
bool Lexer::isAtStartOfMacroExpansion(SourceLocation loc,
                                      const SourceManager &SM,
                                      const LangOptions &LangOpts,
                                      SourceLocation *MacroBegin) {
  assert(loc.isValid() && loc.isMacroID() && "Expected a valid macro loc");

  SourceLocation expansionLoc;
  if (!SM.isAtStartOfImmediateMacroExpansion(loc, &expansionLoc))
    return false;

  if (expansionLoc.isFileID()) {
    // No other macro expansions, this is the first.
    if (MacroBegin)
      *MacroBegin = expansionLoc;
    return true;
  }

  return isAtStartOfMacroExpansion(expansionLoc, SM, LangOpts, MacroBegin);
}

/// Returns true if the given MacroID location points at the last
/// token of the macro expansion.
bool Lexer::isAtEndOfMacroExpansion(SourceLocation loc,
                                    const SourceManager &SM,
                                    const LangOptions &LangOpts,
                                    SourceLocation *MacroEnd) {
  assert(loc.isValid() && loc.isMacroID() && "Expected a valid macro loc");

  SourceLocation spellLoc = SM.getSpellingLoc(loc);
  unsigned tokLen = MeasureTokenLength(spellLoc, SM, LangOpts);
  if (tokLen == 0)
    return false;

  SourceLocation afterLoc = loc.getLocWithOffset(tokLen);
  SourceLocation expansionLoc;
  if (!SM.isAtEndOfImmediateMacroExpansion(afterLoc, &expansionLoc))
    return false;

  if (expansionLoc.isFileID()) {
    // No other macro expansions.
    if (MacroEnd)
      *MacroEnd = expansionLoc;
    return true;
  }

  return isAtEndOfMacroExpansion(expansionLoc, SM, LangOpts, MacroEnd);
}

static CharSourceRange makeRangeFromFileLocs(CharSourceRange Range,
                                             const SourceManager &SM,
                                             const LangOptions &LangOpts) {
  SourceLocation Begin = Range.getBegin();
  SourceLocation End = Range.getEnd();
  assert(Begin.isFileID() && End.isFileID());
  if (Range.isTokenRange()) {
    End = Lexer::getLocForEndOfToken(End, 0, SM,LangOpts);
    if (End.isInvalid())
      return {};
  }

  // Break down the source locations.
  FileID FID;
  unsigned BeginOffs;
  std::tie(FID, BeginOffs) = SM.getDecomposedLoc(Begin);
  if (FID.isInvalid())
    return {};

  unsigned EndOffs;
  if (!SM.isInFileID(End, FID, &EndOffs) ||
      BeginOffs > EndOffs)
    return {};

  return CharSourceRange::getCharRange(Begin, End);
}

// Assumes that `Loc` is in an expansion.
static bool isInExpansionTokenRange(const SourceLocation Loc,
                                    const SourceManager &SM) {
  return SM.getSLocEntry(SM.getFileID(Loc))
      .getExpansion()
      .isExpansionTokenRange();
}

CharSourceRange Lexer::makeFileCharRange(CharSourceRange Range,
                                         const SourceManager &SM,
                                         const LangOptions &LangOpts) {
  SourceLocation Begin = Range.getBegin();
  SourceLocation End = Range.getEnd();
  if (Begin.isInvalid() || End.isInvalid())
    return {};

  if (Begin.isFileID() && End.isFileID())
    return makeRangeFromFileLocs(Range, SM, LangOpts);

  if (Begin.isMacroID() && End.isFileID()) {
    if (!isAtStartOfMacroExpansion(Begin, SM, LangOpts, &Begin))
      return {};
    Range.setBegin(Begin);
    return makeRangeFromFileLocs(Range, SM, LangOpts);
  }

  if (Begin.isFileID() && End.isMacroID()) {
    if (Range.isTokenRange()) {
      if (!isAtEndOfMacroExpansion(End, SM, LangOpts, &End))
        return {};
      // Use the *original* end, not the expanded one in `End`.
      Range.setTokenRange(isInExpansionTokenRange(Range.getEnd(), SM));
    } else if (!isAtStartOfMacroExpansion(End, SM, LangOpts, &End))
      return {};
    Range.setEnd(End);
    return makeRangeFromFileLocs(Range, SM, LangOpts);
  }

  assert(Begin.isMacroID() && End.isMacroID());
  SourceLocation MacroBegin, MacroEnd;
  if (isAtStartOfMacroExpansion(Begin, SM, LangOpts, &MacroBegin) &&
      ((Range.isTokenRange() && isAtEndOfMacroExpansion(End, SM, LangOpts,
                                                        &MacroEnd)) ||
       (Range.isCharRange() && isAtStartOfMacroExpansion(End, SM, LangOpts,
                                                         &MacroEnd)))) {
    Range.setBegin(MacroBegin);
    Range.setEnd(MacroEnd);
    // Use the *original* `End`, not the expanded one in `MacroEnd`.
    if (Range.isTokenRange())
      Range.setTokenRange(isInExpansionTokenRange(End, SM));
    return makeRangeFromFileLocs(Range, SM, LangOpts);
  }

  bool Invalid = false;
  const SrcMgr::SLocEntry &BeginEntry = SM.getSLocEntry(SM.getFileID(Begin),
                                                        &Invalid);
  if (Invalid)
    return {};

  if (BeginEntry.getExpansion().isMacroArgExpansion()) {
    const SrcMgr::SLocEntry &EndEntry = SM.getSLocEntry(SM.getFileID(End),
                                                        &Invalid);
    if (Invalid)
      return {};

    if (EndEntry.getExpansion().isMacroArgExpansion() &&
        BeginEntry.getExpansion().getExpansionLocStart() ==
            EndEntry.getExpansion().getExpansionLocStart()) {
      Range.setBegin(SM.getImmediateSpellingLoc(Begin));
      Range.setEnd(SM.getImmediateSpellingLoc(End));
      return makeFileCharRange(Range, SM, LangOpts);
    }
  }

  return {};
}

StringRef Lexer::getSourceText(CharSourceRange Range,
                               const SourceManager &SM,
                               const LangOptions &LangOpts,
                               bool *Invalid) {
  Range = makeFileCharRange(Range, SM, LangOpts);
  if (Range.isInvalid()) {
    if (Invalid) *Invalid = true;
    return {};
  }

  // Break down the source location.
  std::pair<FileID, unsigned> beginInfo = SM.getDecomposedLoc(Range.getBegin());
  if (beginInfo.first.isInvalid()) {
    if (Invalid) *Invalid = true;
    return {};
  }

  unsigned EndOffs;
  if (!SM.isInFileID(Range.getEnd(), beginInfo.first, &EndOffs) ||
      beginInfo.second > EndOffs) {
    if (Invalid) *Invalid = true;
    return {};
  }

  // Try to the load the file buffer.
  bool invalidTemp = false;
  StringRef file = SM.getBufferData(beginInfo.first, &invalidTemp);
  if (invalidTemp) {
    if (Invalid) *Invalid = true;
    return {};
  }

  if (Invalid) *Invalid = false;
  return file.substr(beginInfo.second, EndOffs - beginInfo.second);
}

StringRef Lexer::getImmediateMacroName(SourceLocation Loc,
                                       const SourceManager &SM,
                                       const LangOptions &LangOpts) {
  assert(Loc.isMacroID() && "Only reasonable to call this on macros");

  // Find the location of the immediate macro expansion.
  while (true) {
    FileID FID = SM.getFileID(Loc);
    const SrcMgr::SLocEntry *E = &SM.getSLocEntry(FID);
    const SrcMgr::ExpansionInfo &Expansion = E->getExpansion();
    Loc = Expansion.getExpansionLocStart();
    if (!Expansion.isMacroArgExpansion())
      break;

    // For macro arguments we need to check that the argument did not come
    // from an inner macro, e.g: "MAC1( MAC2(foo) )"

    // Loc points to the argument id of the macro definition, move to the
    // macro expansion.
    Loc = SM.getImmediateExpansionRange(Loc).getBegin();
    SourceLocation SpellLoc = Expansion.getSpellingLoc();
    if (SpellLoc.isFileID())
      break; // No inner macro.

    // If spelling location resides in the same FileID as macro expansion
    // location, it means there is no inner macro.
    FileID MacroFID = SM.getFileID(Loc);
    if (SM.isInFileID(SpellLoc, MacroFID))
      break;

    // Argument came from inner macro.
    Loc = SpellLoc;
  }

  // Find the spelling location of the start of the non-argument expansion
  // range. This is where the macro name was spelled in order to begin
  // expanding this macro.
  Loc = SM.getSpellingLoc(Loc);

  // Dig out the buffer where the macro name was spelled and the extents of the
  // name so that we can render it into the expansion note.
  std::pair<FileID, unsigned> ExpansionInfo = SM.getDecomposedLoc(Loc);
  unsigned MacroTokenLength = Lexer::MeasureTokenLength(Loc, SM, LangOpts);
  StringRef ExpansionBuffer = SM.getBufferData(ExpansionInfo.first);
  return ExpansionBuffer.substr(ExpansionInfo.second, MacroTokenLength);
}

StringRef Lexer::getImmediateMacroNameForDiagnostics(
    SourceLocation Loc, const SourceManager &SM, const LangOptions &LangOpts) {
  assert(Loc.isMacroID() && "Only reasonable to call this on macros");
  // Walk past macro argument expansions.
  while (SM.isMacroArgExpansion(Loc))
    Loc = SM.getImmediateExpansionRange(Loc).getBegin();

  // If the macro's spelling has no FileID, then it's actually a token paste
  // or stringization (or similar) and not a macro at all.
  if (!SM.getFileEntryForID(SM.getFileID(SM.getSpellingLoc(Loc))))
    return {};

  // Find the spelling location of the start of the non-argument expansion
  // range. This is where the macro name was spelled in order to begin
  // expanding this macro.
  Loc = SM.getSpellingLoc(SM.getImmediateExpansionRange(Loc).getBegin());

  // Dig out the buffer where the macro name was spelled and the extents of the
  // name so that we can render it into the expansion note.
  std::pair<FileID, unsigned> ExpansionInfo = SM.getDecomposedLoc(Loc);
  unsigned MacroTokenLength = Lexer::MeasureTokenLength(Loc, SM, LangOpts);
  StringRef ExpansionBuffer = SM.getBufferData(ExpansionInfo.first);
  return ExpansionBuffer.substr(ExpansionInfo.second, MacroTokenLength);
}

bool Lexer::isAsciiIdentifierContinueChar(char c, const LangOptions &LangOpts) {
  return isAsciiIdentifierContinue(c, LangOpts.DollarIdents);
}

bool Lexer::isNewLineEscaped(const char *BufferStart, const char *Str) {
  assert(isVerticalWhitespace(Str[0]));
  if (Str - 1 < BufferStart)
    return false;

  if ((Str[0] == '\n' && Str[-1] == '\r') ||
      (Str[0] == '\r' && Str[-1] == '\n')) {
    if (Str - 2 < BufferStart)
      return false;
    --Str;
  }
  --Str;

  // Rewind to first non-space character:
  while (Str > BufferStart && isHorizontalWhitespace(*Str))
    --Str;

  return *Str == '\\';
}

StringRef Lexer::getIndentationForLine(SourceLocation Loc,
                                       const SourceManager &SM) {
  if (Loc.isInvalid() || Loc.isMacroID())
    return {};
  std::pair<FileID, unsigned> LocInfo = SM.getDecomposedLoc(Loc);
  if (LocInfo.first.isInvalid())
    return {};
  bool Invalid = false;
  StringRef Buffer = SM.getBufferData(LocInfo.first, &Invalid);
  if (Invalid)
    return {};
  const char *Line = findBeginningOfLine(Buffer, LocInfo.second);
  if (!Line)
    return {};
  StringRef Rest = Buffer.substr(Line - Buffer.data());
  size_t NumWhitespaceChars = Rest.find_first_not_of(" \t");
  return NumWhitespaceChars == StringRef::npos
             ? ""
             : Rest.take_front(NumWhitespaceChars);
}

//===----------------------------------------------------------------------===//
// Diagnostics forwarding code.
//===----------------------------------------------------------------------===//

/// GetMappedTokenLoc - If lexing out of a 'mapped buffer', where we pretend the
/// lexer buffer was all expanded at a single point, perform the mapping.
/// This is currently only used for _Pragma implementation, so it is the slow
/// path of the hot getSourceLocation method.  Do not allow it to be inlined.
static LLVM_ATTRIBUTE_NOINLINE SourceLocation GetMappedTokenLoc(
    Preprocessor &PP, SourceLocation FileLoc, unsigned CharNo, unsigned TokLen);
static SourceLocation GetMappedTokenLoc(Preprocessor &PP,
                                        SourceLocation FileLoc,
                                        unsigned CharNo, unsigned TokLen) {
  assert(FileLoc.isMacroID() && "Must be a macro expansion");

  // Otherwise, we're lexing "mapped tokens".  This is used for things like
  // _Pragma handling.  Combine the expansion location of FileLoc with the
  // spelling location.
  SourceManager &SM = PP.getSourceManager();

  // Create a new SLoc which is expanded from Expansion(FileLoc) but whose
  // characters come from spelling(FileLoc)+Offset.
  SourceLocation SpellingLoc = SM.getSpellingLoc(FileLoc);
  SpellingLoc = SpellingLoc.getLocWithOffset(CharNo);

  // Figure out the expansion loc range, which is the range covered by the
  // original _Pragma(...) sequence.
  CharSourceRange II = SM.getImmediateExpansionRange(FileLoc);

  return SM.createExpansionLoc(SpellingLoc, II.getBegin(), II.getEnd(), TokLen);
}

/// getSourceLocation - Return a source location identifier for the specified
/// offset in the current file.
SourceLocation Lexer::getSourceLocation(unsigned Loc,
                                        unsigned TokLen) const {
  assert(Loc <= BufferSize &&
         "Location out of range for this buffer!");

  // In the normal case, we're just lexing from a simple file buffer, return
  // the file id from FileLoc with the offset specified.
  unsigned CharNo = Loc;
  if (FileLoc.isFileID())
    return FileLoc.getLocWithOffset(CharNo);

  // Otherwise, this is the _Pragma lexer case, which pretends that all of the
  // tokens are lexed from where the _Pragma was defined.
  assert(PP && "This doesn't work on raw lexers");
  return GetMappedTokenLoc(*PP, FileLoc, CharNo, TokLen);
}

/// Diag - Forwarding function for diagnostics.  This translate a source
/// position in the current buffer into a SourceLocation object for rendering.
DiagnosticBuilder Lexer::Diag(unsigned Loc, unsigned DiagID) const {
  return PP->Diag(getSourceLocation(Loc), DiagID);
}

//===----------------------------------------------------------------------===//
// Trigraph and Escaped Newline Handling Code.
//===----------------------------------------------------------------------===//

/// GetTrigraphCharForLetter - Given a character that occurs after a ?? pair,
/// return the decoded trigraph letter it corresponds to, or '\0' if nothing.
static char GetTrigraphCharForLetter(char Letter) {
  switch (Letter) {
  default:   return 0;
  case '=':  return '#';
  case ')':  return ']';
  case '(':  return '[';
  case '!':  return '|';
  case '\'': return '^';
  case '>':  return '}';
  case '/':  return '\\';
  case '<':  return '{';
  case '-':  return '~';
  }
}

/// DecodeTrigraphChar - If the specified character is a legal trigraph when
/// prefixed with ??, emit a trigraph warning.  If trigraphs are enabled,
/// return the result character.  Finally, emit a warning about trigraph use
/// whether trigraphs are enabled or not.
static char DecodeTrigraphChar(const char *CP, Lexer *L, bool Trigraphs) {
  char Res = GetTrigraphCharForLetter(*CP);
  if (!Res || !L) return Res;

  if (!Trigraphs) {
    if (!L->isLexingRawMode())
      L->Diag(CP-2-L->getBuffer().data(), diag::trigraph_ignored);
    return 0;
  }

  if (!L->isLexingRawMode())
    L->Diag(CP-2-L->getBuffer().data(), diag::trigraph_converted) << StringRef(&Res, 1);
  return Res;
}

/// getEscapedNewLineSize - Return the size of the specified escaped newline,
/// or 0 if it is not an escaped newline. P[-1] is known to be a "\" or a
/// trigraph equivalent on entry to this function.
unsigned Lexer::getEscapedNewLineSize(const char *Ptr) {
  unsigned Size = 0;
  while (isWhitespace(Ptr[Size])) {
    ++Size;

    if (Ptr[Size-1] != '\n' && Ptr[Size-1] != '\r')
      continue;

    // If this is a \r\n or \n\r, skip the other half.
    if ((Ptr[Size] == '\r' || Ptr[Size] == '\n') &&
        Ptr[Size-1] != Ptr[Size])
      ++Size;

    return Size;
  }

  // Not an escaped newline, must be a \t or something else.
  return 0;
}

/// SkipEscapedNewLines - If P points to an escaped newline (or a series of
/// them), skip over them and return the first non-escaped-newline found,
/// otherwise return P.
const char *Lexer::SkipEscapedNewLines(const char *P) {
  while (true) {
    const char *AfterEscape;
    if (*P == '\\') {
      AfterEscape = P+1;
    } else if (*P == '?') {
      // If not a trigraph for escape, bail out.
      if (P[1] != '?' || P[2] != '/')
        return P;
      // FIXME: Take LangOpts into account; the language might not
      // support trigraphs.
      AfterEscape = P+3;
    } else {
      return P;
    }

    unsigned NewLineSize = Lexer::getEscapedNewLineSize(AfterEscape);
    if (NewLineSize == 0) return P;
    P = AfterEscape+NewLineSize;
  }
}

Optional<Token> Lexer::findNextToken(SourceLocation Loc,
                                     const SourceManager &SM,
                                     const LangOptions &LangOpts) {
  if (Loc.isMacroID()) {
    if (!Lexer::isAtEndOfMacroExpansion(Loc, SM, LangOpts, &Loc))
      return None;
  }
  Loc = Lexer::getLocForEndOfToken(Loc, 0, SM, LangOpts);

  // Break down the source location.
  std::pair<FileID, unsigned> LocInfo = SM.getDecomposedLoc(Loc);

  // Try to load the file buffer.
  bool InvalidTemp = false;
  StringRef File = SM.getBufferData(LocInfo.first, &InvalidTemp);
  if (InvalidTemp)
    return None;

  const char *TokenBegin = File.data() + LocInfo.second;

  // Lex from the start of the given location.
  Lexer lexer(SM.getLocForStartOfFile(LocInfo.first), LangOpts, File.begin(),
                                      TokenBegin, File.end());
  // Find the token.
  Token Tok;
  lexer.LexFromRawLexer(Tok);
  return Tok;
}

/// Checks that the given token is the first token that occurs after the
/// given location (this excludes comments and whitespace). Returns the location
/// immediately after the specified token. If the token is not found or the
/// location is inside a macro, the returned source location will be invalid.
SourceLocation Lexer::findLocationAfterToken(
    SourceLocation Loc, tok::TokenKind TKind, const SourceManager &SM,
    const LangOptions &LangOpts, bool SkipTrailingWhitespaceAndNewLine) {
  Optional<Token> Tok = findNextToken(Loc, SM, LangOpts);
  if (!Tok || Tok->isNot(TKind))
    return {};
  SourceLocation TokenLoc = Tok->getLocation();

  // Calculate how much whitespace needs to be skipped if any.
  unsigned NumWhitespaceChars = 0;
  if (SkipTrailingWhitespaceAndNewLine) {
    const char *TokenEnd = SM.getCharacterData(TokenLoc) + Tok->getLength();
    unsigned char C = *TokenEnd;
    while (isHorizontalWhitespace(C)) {
      C = *(++TokenEnd);
      NumWhitespaceChars++;
    }

    // Skip \r, \n, \r\n, or \n\r
    if (C == '\n' || C == '\r') {
      char PrevC = C;
      C = *(++TokenEnd);
      NumWhitespaceChars++;
      if ((C == '\n' || C == '\r') && C != PrevC)
        NumWhitespaceChars++;
    }
  }

  return TokenLoc.getLocWithOffset(Tok->getLength() + NumWhitespaceChars);
}

/// getCharAndSizeSlow - Peek a single 'character' from the specified buffer,
/// get its size, and return it.  This is tricky in several cases:
///   1. If currently at the start of a trigraph, we warn about the trigraph,
///      then either return the trigraph (skipping 3 chars) or the '?',
///      depending on whether trigraphs are enabled or not.
///   2. If this is an escaped newline (potentially with whitespace between
///      the backslash and newline), implicitly skip the newline and return
///      the char after it.
///
/// This handles the slow/uncommon case of the getCharAndSize method.  Here we
/// know that we can accumulate into Size, and that we have already incremented
/// Ptr by Size bytes.
///
/// NOTE: When this method is updated, getCharAndSizeSlowNoWarn (below) should
/// be updated to match.
char Lexer::getCharAndSizeSlow(unsigned Offset, unsigned &Size,
                               Token *Tok) {
  // If we have a slash, look for an escaped newline.
  if (BufferStart[Offset] == '\\') {
    ++Size;
    ++Offset;
Slash:
    // Common case, backslash-char where the char is not whitespace.
    if (!isWhitespace(BufferStart[Offset])) return '\\';

    // See if we have optional whitespace characters between the slash and
    // newline.
    if (unsigned EscapedNewLineSize = getEscapedNewLineSize(&BufferStart[Offset])) {
      // Remember that this token needs to be cleaned.
      if (Tok) Tok->setFlag(Token::NeedsCleaning);

      // Warn if there was whitespace between the backslash and newline.
      if (BufferStart[Offset] != '\n' && BufferStart[Offset] != '\r' && Tok && !isLexingRawMode())
        Diag(Offset, diag::backslash_newline_space);

      // Found backslash<whitespace><newline>.  Parse the char after it.
      Size += EscapedNewLineSize;
      Offset  += EscapedNewLineSize;

      if (BufferStart[Offset] == 0 && Offset == BufferSize) 
        TryExpandBuffer();

      // Use slow version to accumulate a correct size field.
      return getCharAndSizeSlow(Offset, Size, Tok);
    }

    // Otherwise, this is not an escaped newline, just return the slash.
    return '\\';
  }

  // If this is a trigraph, process it.
  if (BufferStart[Offset] == '?' && BufferStart[Offset+1] == '?') {
    // If this is actually a legal trigraph (not something like "??x"), emit
    // a trigraph warning.  If so, and if trigraphs are enabled, return it.
    if (char C = DecodeTrigraphChar(&BufferStart[Offset + 2], Tok ? this : nullptr,
                                    LangOpts.Trigraphs)) {
      // Remember that this token needs to be cleaned.
      if (Tok) Tok->setFlag(Token::NeedsCleaning);

      Offset += 3;
      Size += 3;
      if (C == '\\') goto Slash;
      return C;
    }
  }

  // If this is neither, return a single character.
  ++Size;
  return BufferStart[Offset];
}

/// getCharAndSizeSlowNoWarn - Handle the slow/uncommon case of the
/// getCharAndSizeNoWarn method.  Here we know that we can accumulate into Size,
/// and that we have already incremented Ptr by Size bytes.
///
/// NOTE: When this method is updated, getCharAndSizeSlow (above) should
/// be updated to match.
char Lexer::getCharAndSizeSlowNoWarn(const char *Ptr, unsigned &Size,
                                     const LangOptions &LangOpts) {
  // If we have a slash, look for an escaped newline.
  if (Ptr[0] == '\\') {
    ++Size;
    ++Ptr;
Slash:
    // Common case, backslash-char where the char is not whitespace.
    if (!isWhitespace(Ptr[0])) return '\\';

    // See if we have optional whitespace characters followed by a newline.
    if (unsigned EscapedNewLineSize = getEscapedNewLineSize(Ptr)) {
      // Found backslash<whitespace><newline>.  Parse the char after it.
      Size += EscapedNewLineSize;
      Ptr  += EscapedNewLineSize;

      // Use slow version to accumulate a correct size field.
      return getCharAndSizeSlowNoWarn(Ptr, Size, LangOpts);
    }

    // Otherwise, this is not an escaped newline, just return the slash.
    return '\\';
  }

  // If this is a trigraph, process it.
  if (LangOpts.Trigraphs && Ptr[0] == '?' && Ptr[1] == '?') {
    // If this is actually a legal trigraph (not something like "??x"), return
    // it.
    if (char C = GetTrigraphCharForLetter(Ptr[2])) {
      Ptr += 3;
      Size += 3;
      if (C == '\\') goto Slash;
      return C;
    }
  }

  // If this is neither, return a single character.
  ++Size;
  return *Ptr;
}

//===----------------------------------------------------------------------===//
// Helper methods for lexing.
//===----------------------------------------------------------------------===//

/// Routine that indiscriminately sets the offset into the source file.
void Lexer::SetByteOffset(unsigned Offset, bool StartOfLine) {
  BufferOffset = Offset;
  if (Offset > BufferSize)
    Offset = BufferSize;
  // FIXME: What exactly does the StartOfLine bit mean?  There are two
  // possible meanings for the "start" of the line: the first token on the
  // unexpanded line, or the first token on the expanded line.
  IsAtStartOfLine = StartOfLine;
  IsAtPhysicalStartOfLine = StartOfLine;
}

static bool isUnicodeWhitespace(uint32_t Codepoint) {
  static const llvm::sys::UnicodeCharSet UnicodeWhitespaceChars(
      UnicodeWhitespaceCharRanges);
  return UnicodeWhitespaceChars.contains(Codepoint);
}

static bool isAllowedIDChar(uint32_t C, const LangOptions &LangOpts) {
  if (LangOpts.AsmPreprocessor) {
    return false;
  } else if (LangOpts.DollarIdents && '$' == C) {
    return true;
  } else if (LangOpts.CPlusPlus || LangOpts.C2x) {
    // A non-leading codepoint must have the XID_Continue property.
    // XIDContinueRanges doesn't contains characters also in XIDStartRanges,
    // so we need to check both tables.
    // '_' doesn't have the XID_Continue property but is allowed in C and C++.
    static const llvm::sys::UnicodeCharSet XIDStartChars(XIDStartRanges);
    static const llvm::sys::UnicodeCharSet XIDContinueChars(XIDContinueRanges);
    return C == '_' || XIDStartChars.contains(C) ||
           XIDContinueChars.contains(C);
  } else if (LangOpts.C11) {
    static const llvm::sys::UnicodeCharSet C11AllowedIDChars(
        C11AllowedIDCharRanges);
    return C11AllowedIDChars.contains(C);
  } else {
    static const llvm::sys::UnicodeCharSet C99AllowedIDChars(
        C99AllowedIDCharRanges);
    return C99AllowedIDChars.contains(C);
  }
}

static bool isAllowedInitiallyIDChar(uint32_t C, const LangOptions &LangOpts) {
  assert(C > 0x7F && "isAllowedInitiallyIDChar called with an ASCII codepoint");
  if (LangOpts.AsmPreprocessor) {
    return false;
  }
  if (LangOpts.CPlusPlus || LangOpts.C2x) {
    static const llvm::sys::UnicodeCharSet XIDStartChars(XIDStartRanges);
    return XIDStartChars.contains(C);
  }
  if (!isAllowedIDChar(C, LangOpts))
    return false;
  if (LangOpts.C11) {
    static const llvm::sys::UnicodeCharSet C11DisallowedInitialIDChars(
        C11DisallowedInitialIDCharRanges);
    return !C11DisallowedInitialIDChars.contains(C);
  }
  static const llvm::sys::UnicodeCharSet C99DisallowedInitialIDChars(
      C99DisallowedInitialIDCharRanges);
  return !C99DisallowedInitialIDChars.contains(C);
}

static inline CharSourceRange makeCharRange(Lexer &L, unsigned Begin,
                                            unsigned End) {
  return CharSourceRange::getCharRange(L.getSourceLocation(Begin),
                                       L.getSourceLocation(End));
}

static void maybeDiagnoseIDCharCompat(DiagnosticsEngine &Diags, uint32_t C,
                                      CharSourceRange Range, bool IsFirst) {
  // Check C99 compatibility.
  if (!Diags.isIgnored(diag::warn_c99_compat_unicode_id, Range.getBegin())) {
    enum {
      CannotAppearInIdentifier = 0,
      CannotStartIdentifier
    };

    static const llvm::sys::UnicodeCharSet C99AllowedIDChars(
        C99AllowedIDCharRanges);
    static const llvm::sys::UnicodeCharSet C99DisallowedInitialIDChars(
        C99DisallowedInitialIDCharRanges);
    if (!C99AllowedIDChars.contains(C)) {
      Diags.Report(Range.getBegin(), diag::warn_c99_compat_unicode_id)
        << Range
        << CannotAppearInIdentifier;
    } else if (IsFirst && C99DisallowedInitialIDChars.contains(C)) {
      Diags.Report(Range.getBegin(), diag::warn_c99_compat_unicode_id)
        << Range
        << CannotStartIdentifier;
    }
  }
}

/// After encountering UTF-8 character C and interpreting it as an identifier
/// character, check whether it's a homoglyph for a common non-identifier
/// source character that is unlikely to be an intentional identifier
/// character and warn if so.
static void maybeDiagnoseUTF8Homoglyph(DiagnosticsEngine &Diags, uint32_t C,
                                       CharSourceRange Range) {
  // FIXME: Handle Unicode quotation marks (smart quotes, fullwidth quotes).
  struct HomoglyphPair {
    uint32_t Character;
    char LooksLike;
    bool operator<(HomoglyphPair R) const { return Character < R.Character; }
  };
  static constexpr HomoglyphPair SortedHomoglyphs[] = {
    {U'\u00ad', 0},   // SOFT HYPHEN
    {U'\u01c3', '!'}, // LATIN LETTER RETROFLEX CLICK
    {U'\u037e', ';'}, // GREEK QUESTION MARK
    {U'\u200b', 0},   // ZERO WIDTH SPACE
    {U'\u200c', 0},   // ZERO WIDTH NON-JOINER
    {U'\u200d', 0},   // ZERO WIDTH JOINER
    {U'\u2060', 0},   // WORD JOINER
    {U'\u2061', 0},   // FUNCTION APPLICATION
    {U'\u2062', 0},   // INVISIBLE TIMES
    {U'\u2063', 0},   // INVISIBLE SEPARATOR
    {U'\u2064', 0},   // INVISIBLE PLUS
    {U'\u2212', '-'}, // MINUS SIGN
    {U'\u2215', '/'}, // DIVISION SLASH
    {U'\u2216', '\\'}, // SET MINUS
    {U'\u2217', '*'}, // ASTERISK OPERATOR
    {U'\u2223', '|'}, // DIVIDES
    {U'\u2227', '^'}, // LOGICAL AND
    {U'\u2236', ':'}, // RATIO
    {U'\u223c', '~'}, // TILDE OPERATOR
    {U'\ua789', ':'}, // MODIFIER LETTER COLON
    {U'\ufeff', 0},   // ZERO WIDTH NO-BREAK SPACE
    {U'\uff01', '!'}, // FULLWIDTH EXCLAMATION MARK
    {U'\uff03', '#'}, // FULLWIDTH NUMBER SIGN
    {U'\uff04', '$'}, // FULLWIDTH DOLLAR SIGN
    {U'\uff05', '%'}, // FULLWIDTH PERCENT SIGN
    {U'\uff06', '&'}, // FULLWIDTH AMPERSAND
    {U'\uff08', '('}, // FULLWIDTH LEFT PARENTHESIS
    {U'\uff09', ')'}, // FULLWIDTH RIGHT PARENTHESIS
    {U'\uff0a', '*'}, // FULLWIDTH ASTERISK
    {U'\uff0b', '+'}, // FULLWIDTH ASTERISK
    {U'\uff0c', ','}, // FULLWIDTH COMMA
    {U'\uff0d', '-'}, // FULLWIDTH HYPHEN-MINUS
    {U'\uff0e', '.'}, // FULLWIDTH FULL STOP
    {U'\uff0f', '/'}, // FULLWIDTH SOLIDUS
    {U'\uff1a', ':'}, // FULLWIDTH COLON
    {U'\uff1b', ';'}, // FULLWIDTH SEMICOLON
    {U'\uff1c', '<'}, // FULLWIDTH LESS-THAN SIGN
    {U'\uff1d', '='}, // FULLWIDTH EQUALS SIGN
    {U'\uff1e', '>'}, // FULLWIDTH GREATER-THAN SIGN
    {U'\uff1f', '?'}, // FULLWIDTH QUESTION MARK
    {U'\uff20', '@'}, // FULLWIDTH COMMERCIAL AT
    {U'\uff3b', '['}, // FULLWIDTH LEFT SQUARE BRACKET
    {U'\uff3c', '\\'}, // FULLWIDTH REVERSE SOLIDUS
    {U'\uff3d', ']'}, // FULLWIDTH RIGHT SQUARE BRACKET
    {U'\uff3e', '^'}, // FULLWIDTH CIRCUMFLEX ACCENT
    {U'\uff5b', '{'}, // FULLWIDTH LEFT CURLY BRACKET
    {U'\uff5c', '|'}, // FULLWIDTH VERTICAL LINE
    {U'\uff5d', '}'}, // FULLWIDTH RIGHT CURLY BRACKET
    {U'\uff5e', '~'}, // FULLWIDTH TILDE
    {0, 0}
  };
  auto Homoglyph =
      std::lower_bound(std::begin(SortedHomoglyphs),
                       std::end(SortedHomoglyphs) - 1, HomoglyphPair{C, '\0'});
  if (Homoglyph->Character == C) {
    llvm::SmallString<5> CharBuf;
    {
      llvm::raw_svector_ostream CharOS(CharBuf);
      llvm::write_hex(CharOS, C, llvm::HexPrintStyle::Upper, 4);
    }
    if (Homoglyph->LooksLike) {
      const char LooksLikeStr[] = {Homoglyph->LooksLike, 0};
      Diags.Report(Range.getBegin(), diag::warn_utf8_symbol_homoglyph)
          << Range << CharBuf << LooksLikeStr;
    } else {
      Diags.Report(Range.getBegin(), diag::warn_utf8_symbol_zero_width)
          << Range << CharBuf;
    }
  }
}

static void diagnoseInvalidUnicodeCodepointInIdentifier(
    DiagnosticsEngine &Diags, const LangOptions &LangOpts, uint32_t CodePoint,
    CharSourceRange Range, bool IsFirst) {
  if (isASCII(CodePoint))
    return;

  bool IsIDStart = isAllowedInitiallyIDChar(CodePoint, LangOpts);
  bool IsIDContinue = IsIDStart || isAllowedIDChar(CodePoint, LangOpts);

  if ((IsFirst && IsIDStart) || (!IsFirst && IsIDContinue))
    return;

  bool InvalidOnlyAtStart = IsFirst && !IsIDStart && IsIDContinue;

  llvm::SmallString<5> CharBuf;
  llvm::raw_svector_ostream CharOS(CharBuf);
  llvm::write_hex(CharOS, CodePoint, llvm::HexPrintStyle::Upper, 4);

  if (!IsFirst || InvalidOnlyAtStart) {
    Diags.Report(Range.getBegin(), diag::err_character_not_allowed_identifier)
        << Range << CharBuf << int(InvalidOnlyAtStart)
        << FixItHint::CreateRemoval(Range);
  } else {
    Diags.Report(Range.getBegin(), diag::err_character_not_allowed)
        << Range << CharBuf << FixItHint::CreateRemoval(Range);
  }
}

bool Lexer::tryConsumeIdentifierUCN(unsigned &CurOffset, unsigned Size,
                                    Token &Result) {
  unsigned UCNOffset = CurOffset + Size;
  uint32_t CodePoint = tryReadUCN(UCNOffset, CurOffset, /*Token=*/nullptr);
  if (CodePoint == 0) {
    return false;
  }

  if (!isAllowedIDChar(CodePoint, LangOpts)) {
    if (isASCII(CodePoint) || isUnicodeWhitespace(CodePoint))
      return false;
    if (!isLexingRawMode() && !ParsingPreprocessorDirective &&
        !PP->isPreprocessedOutput())
      diagnoseInvalidUnicodeCodepointInIdentifier(
          PP->getDiagnostics(), LangOpts, CodePoint,
          makeCharRange(*this, CurOffset, UCNOffset),
          /*IsFirst=*/false);

    // We got a unicode codepoint that is neither a space nor a
    // a valid identifier part.
    // Carry on as if the codepoint was valid for recovery purposes.
  } else if (!isLexingRawMode())
    maybeDiagnoseIDCharCompat(PP->getDiagnostics(), CodePoint,
                              makeCharRange(*this, CurOffset, UCNOffset),
                              /*IsFirst=*/false);

  Result.setFlag(Token::HasUCN);
  if ((UCNOffset - CurOffset ==  6 && BufferStart[CurOffset+1] == 'u') ||
      (UCNOffset - CurOffset == 10 && BufferStart[CurOffset+1] == 'U'))
    CurOffset = UCNOffset;
  else
    while (CurOffset != UCNOffset)
      (void)getAndAdvanceChar(CurOffset, Result);
  return true;
}

bool Lexer::tryConsumeIdentifierUTF8Char(unsigned &CurOffset) {
  const char* UnicodePtr = &BufferStart[CurOffset];
  llvm::UTF32 CodePoint;
  llvm::ConversionResult Result =
      llvm::convertUTF8Sequence((const llvm::UTF8 **)&UnicodePtr,
                                (const llvm::UTF8 *)&BufferStart[BufferSize],
                                &CodePoint,
                                llvm::strictConversion);
  if (Result != llvm::conversionOK)
    return false;

  if (!isAllowedIDChar(static_cast<uint32_t>(CodePoint), LangOpts)) {
    if (isASCII(CodePoint) || isUnicodeWhitespace(CodePoint))
      return false;

    if (!isLexingRawMode() && !ParsingPreprocessorDirective &&
        !PP->isPreprocessedOutput())
      diagnoseInvalidUnicodeCodepointInIdentifier(
          PP->getDiagnostics(), LangOpts, CodePoint,
          makeCharRange(*this, CurOffset, UnicodePtr-BufferStart), /*IsFirst=*/false);
    // We got a unicode codepoint that is neither a space nor a
    // a valid identifier part. Carry on as if the codepoint was
    // valid for recovery purposes.
  } else if (!isLexingRawMode()) {
    maybeDiagnoseIDCharCompat(PP->getDiagnostics(), CodePoint,
                              makeCharRange(*this, CurOffset, UnicodePtr-BufferStart),
                              /*IsFirst=*/false);
    maybeDiagnoseUTF8Homoglyph(PP->getDiagnostics(), CodePoint,
                               makeCharRange(*this, CurOffset, UnicodePtr-BufferStart));
  }

  CurOffset = UnicodePtr-BufferStart;
  return true;
}

bool Lexer::LexUnicodeIdentifierStart(Token &Result, uint32_t C,
                                      unsigned CurOffset) {
  if (isAllowedInitiallyIDChar(C, LangOpts)) {
    if (!isLexingRawMode() && !ParsingPreprocessorDirective &&
        !PP->isPreprocessedOutput()) {
      maybeDiagnoseIDCharCompat(PP->getDiagnostics(), C,
                                makeCharRange(*this, BufferOffset, CurOffset),
                                /*IsFirst=*/true);
      maybeDiagnoseUTF8Homoglyph(PP->getDiagnostics(), C,
                                 makeCharRange(*this, BufferOffset, CurOffset));
    }

    MIOpt.ReadToken();
    return LexIdentifierContinue(Result, CurOffset);
  }

  if (!isLexingRawMode() && !ParsingPreprocessorDirective &&
      !PP->isPreprocessedOutput() && !isASCII(BufferStart[BufferOffset]) &&
      !isAllowedInitiallyIDChar(C, LangOpts) && !isUnicodeWhitespace(C)) {
    // Non-ASCII characters tend to creep into source code unintentionally.
    // Instead of letting the parser complain about the unknown token,
    // just drop the character.
    // Note that we can /only/ do this when the non-ASCII character is actually
    // spelled as Unicode, not written as a UCN. The standard requires that
    // we not throw away any possible preprocessor tokens, but there's a
    // loophole in the mapping of Unicode characters to basic character set
    // characters that allows us to map these particular characters to, say,
    // whitespace.
    diagnoseInvalidUnicodeCodepointInIdentifier(
        PP->getDiagnostics(), LangOpts, C,
        makeCharRange(*this, BufferOffset, CurOffset), /*IsStart*/ true);
    BufferOffset = CurOffset;
    return false;
  }

  // Otherwise, we have an explicit UCN or a character that's unlikely to show
  // up by accident.
  MIOpt.ReadToken();
  FormTokenWithChars(Result, CurOffset, tok::unknown);
  return true;
}

bool Lexer::LexIdentifierContinue(Token &Result, unsigned CurOffset) {
  // Match [_A-Za-z0-9]*, we have already matched an identifier start.
  while (true) {
    unsigned char C = BufferStart[CurOffset];
    // Fast path.
    if (isAsciiIdentifierContinue(C)) {
      ++CurOffset;
      continue;
    }

    unsigned Size;
    // Slow path: handle trigraph, unicode codepoints, UCNs.
    C = getCharAndSize(CurOffset, Size);
    if (isAsciiIdentifierContinue(C)) {
      CurOffset = ConsumeChar(CurOffset, Size, Result);
      continue;
    }
    if (C == '$') {
      // If we hit a $ and they are not supported in identifiers, we are done.
      if (!LangOpts.DollarIdents)
        break;
      // Otherwise, emit a diagnostic and continue.
      if (!isLexingRawMode())
        Diag(CurOffset, diag::ext_dollar_in_identifier);
      CurOffset = ConsumeChar(CurOffset, Size, Result);
      continue;
    }
    if (C == '\\' && tryConsumeIdentifierUCN(CurOffset, Size, Result))
      continue;
    if (!isASCII(C) && tryConsumeIdentifierUTF8Char(CurOffset))
      continue;
    // Neither an expected Unicode codepoint nor a UCN.
    break;
  }

  const char *IdStart = BufferStart + BufferOffset;
  unsigned TokLen = CurOffset - BufferOffset;
  FormTokenWithChars(Result, CurOffset, tok::raw_identifier);
  SetTokString(Result, StringRef(IdStart, TokLen));

  // If we are in raw mode, return this identifier raw.  There is no need to
  // look up identifier information or attempt to macro expand it.
  if (LexingRawMode)
    return true;

  // Fill in Result.IdentifierInfo and update the token kind,
  // looking up the identifier in the identifier table.
  IdentifierInfo *II = PP->LookUpIdentifierInfo(Result);
  // Note that we have to call PP->LookUpIdentifierInfo() even for code
  // completion, it writes IdentifierInfo into Result, and callers rely on it.

  // If the completion point is at the end of an identifier, we want to treat
  // the identifier as incomplete even if it resolves to a macro or a keyword.
  // This allows e.g. 'class^' to complete to 'classifier'.
  if (isCodeCompletionPoint(CurOffset)) {
    // Return the code-completion token.
    Result.setKind(tok::code_completion);
    // Skip the code-completion char and all immediate identifier characters.
    // This ensures we get consistent behavior when completing at any point in
    // an identifier (i.e. at the start, in the middle, at the end). Note that
    // only simple cases (i.e. [a-zA-Z0-9_]) are supported to keep the code
    // simpler.
    assert(BufferStart[CurOffset] == 0 && "Completion character must be 0");
    ++CurOffset;
    // Note that code completion token is not added as a separate character
    // when the completion point is at the end of the buffer. Therefore, we need
    // to check if the buffer has ended.
    if (CurOffset < BufferSize) {
      while (isAsciiIdentifierContinue(BufferStart[CurOffset]))
        ++CurOffset;
    }
    BufferOffset = CurOffset;
    return true;
  }

  // Finally, now that we know we have an identifier, pass this off to the
  // preprocessor, which may macro expand it or something.
  if (II->isHandleIdentifierCase())
    return PP->HandleIdentifier(Result);

  return true;
}

/// isHexaLiteral - Return true if Start points to a hex constant.
/// in microsoft mode (where this is supposed to be several different tokens).
bool Lexer::isHexaLiteral(unsigned Start, const LangOptions &LangOpts) {
  unsigned Size;
  char C1 = Lexer::getCharAndSizeNoWarn(&BufferStart[Start], Size, LangOpts);
  if (C1 != '0')
    return false;
  char C2 = Lexer::getCharAndSizeNoWarn(&BufferStart[Start + Size], Size, LangOpts);
  return (C2 == 'x' || C2 == 'X');
}

/// LexNumericConstant - Lex the remainder of a integer or floating point
/// constant. From[-1] is the first character lexed.  Return the end of the
/// constant.
bool Lexer::LexNumericConstant(Token &Result, unsigned CurOffset) {
  unsigned Size;
  char C = getCharAndSize(CurOffset, Size);
  char PrevCh = 0;
  while (isPreprocessingNumberBody(C)) {
    CurOffset = ConsumeChar(CurOffset, Size, Result);
    PrevCh = C;
    C = getCharAndSize(CurOffset, Size);
  }

  // If we fell out, check for a sign, due to 1e+12.  If we have one, continue.
  if ((C == '-' || C == '+') && (PrevCh == 'E' || PrevCh == 'e')) {
    // If we are in Microsoft mode, don't continue if the constant is hex.
    // For example, MSVC will accept the following as 3 tokens: 0x1234567e+1
    if (!LangOpts.MicrosoftExt || !isHexaLiteral(BufferOffset, LangOpts))
      return LexNumericConstant(Result, ConsumeChar(CurOffset, Size, Result));
  }

  // If we have a hex FP constant, continue.
  if ((C == '-' || C == '+') && (PrevCh == 'P' || PrevCh == 'p')) {
    // Outside C99 and C++17, we accept hexadecimal floating point numbers as a
    // not-quite-conforming extension. Only do so if this looks like it's
    // actually meant to be a hexfloat, and not if it has a ud-suffix.
    bool IsHexFloat = true;
    if (!LangOpts.C99) {
      if (!isHexaLiteral(BufferOffset, LangOpts))
        IsHexFloat = false;
      else if (!LangOpts.CPlusPlus17 &&
               std::find(BufferStart + BufferOffset, BufferStart + CurOffset, '_') != BufferStart + CurOffset)
        IsHexFloat = false;
    }
    if (IsHexFloat)
      return LexNumericConstant(Result, ConsumeChar(CurOffset, Size, Result));
  }

  // If we have a digit separator, continue.
  if (C == '\'' && (LangOpts.CPlusPlus14 || LangOpts.C2x)) {
    unsigned NextSize;
    char Next = getCharAndSizeNoWarn(&BufferStart[CurOffset + Size], NextSize, LangOpts);
    if (isAsciiIdentifierContinue(Next)) {
      if (!isLexingRawMode())
        Diag(CurOffset, LangOpts.CPlusPlus
                         ? diag::warn_cxx11_compat_digit_separator
                         : diag::warn_c2x_compat_digit_separator);
      CurOffset = ConsumeChar(CurOffset, Size, Result);
      CurOffset = ConsumeChar(CurOffset, NextSize, Result);
      return LexNumericConstant(Result, CurOffset);
    }
  }

  // If we have a UCN or UTF-8 character (perhaps in a ud-suffix), continue.
  if (C == '\\' && tryConsumeIdentifierUCN(CurOffset, Size, Result))
    return LexNumericConstant(Result, CurOffset);
  if (!isASCII(C) && tryConsumeIdentifierUTF8Char(CurOffset))
    return LexNumericConstant(Result, CurOffset);

  // Update the location of token as well as BufferPtr.
  const char *TokStart = BufferStart + BufferOffset;
  unsigned TokLen = CurOffset - BufferOffset;
  FormTokenWithChars(Result, CurOffset, tok::numeric_constant);
  SetTokString(Result, StringRef(TokStart, TokLen));
  return true;
}

/// LexUDSuffix - Lex the ud-suffix production for user-defined literal suffixes
/// in C++11, or warn on a ud-suffix in C++98.
unsigned Lexer::LexUDSuffix(Token &Result, unsigned CurOffset,
                               bool IsStringLiteral) {
  assert(LangOpts.CPlusPlus);

  // Maximally munch an identifier.
  unsigned Size;
  char C = getCharAndSize(CurOffset, Size);
  bool Consumed = false;

  if (!isAsciiIdentifierStart(C)) {
    if (C == '\\' && tryConsumeIdentifierUCN(CurOffset, Size, Result))
      Consumed = true;
    else if (!isASCII(C) && tryConsumeIdentifierUTF8Char(CurOffset))
      Consumed = true;
    else
      return CurOffset;
  }

  if (!LangOpts.CPlusPlus11) {
    if (!isLexingRawMode())
      Diag(CurOffset,
           C == '_' ? diag::warn_cxx11_compat_user_defined_literal
                    : diag::warn_cxx11_compat_reserved_user_defined_literal)
        << FixItHint::CreateInsertion(getSourceLocation(CurOffset), " ");
    return CurOffset;
  }

  // C++11 [lex.ext]p10, [usrlit.suffix]p1: A program containing a ud-suffix
  // that does not start with an underscore is ill-formed. As a conforming
  // extension, we treat all such suffixes as if they had whitespace before
  // them. We assume a suffix beginning with a UCN or UTF-8 character is more
  // likely to be a ud-suffix than a macro, however, and accept that.
  if (!Consumed) {
    bool IsUDSuffix = false;
    if (C == '_')
      IsUDSuffix = true;
    else if (IsStringLiteral && LangOpts.CPlusPlus14) {
      // In C++1y, we need to look ahead a few characters to see if this is a
      // valid suffix for a string literal or a numeric literal (this could be
      // the 'operator""if' defining a numeric literal operator).
      const unsigned MaxStandardSuffixLength = 3;
      char Buffer[MaxStandardSuffixLength] = { C };
      unsigned Consumed = Size;
      unsigned Chars = 1;
      while (true) {
        unsigned NextSize;
        char Next = getCharAndSizeNoWarn(&BufferStart[CurOffset] + Consumed, NextSize, LangOpts);
        if (!isAsciiIdentifierContinue(Next)) {
          // End of suffix. Check whether this is on the allowed list.
          const StringRef CompleteSuffix(Buffer, Chars);
          IsUDSuffix =
              StringLiteralParser::isValidUDSuffix(LangOpts, CompleteSuffix);
          break;
        }

        if (Chars == MaxStandardSuffixLength)
          // Too long: can't be a standard suffix.
          break;

        Buffer[Chars++] = Next;
        Consumed += NextSize;
      }
    }

    if (!IsUDSuffix) {
      if (!isLexingRawMode())
        Diag(CurOffset, LangOpts.MSVCCompat
                         ? diag::ext_ms_reserved_user_defined_literal
                         : diag::ext_reserved_user_defined_literal)
            << FixItHint::CreateInsertion(getSourceLocation(CurOffset), " ");
      return CurOffset;
    }

    CurOffset = ConsumeChar(CurOffset, Size, Result);
  }

  Result.setFlag(Token::HasUDSuffix);
  while (true) {
    C = getCharAndSize(CurOffset, Size);
    if (isAsciiIdentifierContinue(C)) {
      CurOffset = ConsumeChar(CurOffset, Size, Result);
    } else if (C == '\\' && tryConsumeIdentifierUCN(CurOffset, Size, Result)) {
    } else if (!isASCII(C) && tryConsumeIdentifierUTF8Char(CurOffset)) {
    } else
      break;
  }

  return CurOffset;
}

/// LexStringLiteral - Lex the remainder of a string literal, after having lexed
/// either " or L" or u8" or u" or U".
bool Lexer::LexStringLiteral(Token &Result, unsigned CurOffset,
                             tok::TokenKind Kind) {
  unsigned AfterQuote = CurOffset;
  // Does this string contain the \0 character?
  llvm::Optional<unsigned> NulCharacter = llvm::None;

  if (!isLexingRawMode() &&
      (Kind == tok::utf8_string_literal ||
       Kind == tok::utf16_string_literal ||
       Kind == tok::utf32_string_literal))
    Diag(BufferOffset, LangOpts.CPlusPlus ? diag::warn_cxx98_compat_unicode_literal
                                       : diag::warn_c99_compat_unicode_literal);

  char C = getAndAdvanceChar(CurOffset, Result);
  while (C != '"') {
    // Skip escaped characters.  Escaped newlines will already be processed by
    // getAndAdvanceChar.
    if (C == '\\')
      C = getAndAdvanceChar(CurOffset, Result);

    if (C == '\n' || C == '\r' ||             // Newline.
        (C == 0 && CurOffset-1 == BufferSize)) {  // End of file.
      if (!isLexingRawMode() && !LangOpts.AsmPreprocessor)
        Diag(BufferOffset, diag::ext_unterminated_char_or_string) << 1;
      FormTokenWithChars(Result, CurOffset-1, tok::unknown);
      return true;
    }

    if (C == 0) {
      if (isCodeCompletionPoint(CurOffset-1)) {
        if (ParsingFilename)
          codeCompleteIncludedFile(AfterQuote, CurOffset - 1, /*IsAngled=*/false);
        else
          PP->CodeCompleteNaturalLanguage();
        FormTokenWithChars(Result, CurOffset - 1, tok::unknown);
        cutOffLexing();
        return true;
      }

      NulCharacter = CurOffset-1;
    }
    C = getAndAdvanceChar(CurOffset, Result);
  }

  // If we are in C++11, lex the optional ud-suffix.
  if (LangOpts.CPlusPlus)
    CurOffset = LexUDSuffix(Result, CurOffset, true);

  // If a nul character existed in the string, warn about it.
  if (NulCharacter.hasValue() && !isLexingRawMode())
    Diag(*NulCharacter, diag::null_in_char_or_string) << 1;

  // Update the location of the token as well as the BufferPtr instance var.
  const char *TokStart = BufferStart + BufferOffset;
  unsigned TokLen = CurOffset - BufferOffset;

  FormTokenWithChars(Result, CurOffset, Kind);
  SetTokString(Result, StringRef(TokStart, TokLen));
  return true;
}

/// LexRawStringLiteral - Lex the remainder of a raw string literal, after
/// having lexed R", LR", u8R", uR", or UR".
bool Lexer::LexRawStringLiteral(Token &Result, unsigned CurOffset,
                                tok::TokenKind Kind) {
  // This function doesn't use getAndAdvanceChar because C++0x [lex.pptoken]p3:
  //  Between the initial and final double quote characters of the raw string,
  //  any transformations performed in phases 1 and 2 (trigraphs,
  //  universal-character-names, and line splicing) are reverted.

  if (!isLexingRawMode())
    Diag(BufferOffset, diag::warn_cxx98_compat_raw_string_literal);

  unsigned PrefixLen = 0;

  while (PrefixLen != 16 && isRawStringDelimBody(BufferStart[CurOffset + PrefixLen]))
    ++PrefixLen;

  // If the last character was not a '(', then we didn't lex a valid delimiter.
  if (BufferStart[CurOffset + PrefixLen] != '(') {
    if (!isLexingRawMode()) {
      unsigned PrefixEnd = CurOffset + PrefixLen;
      if (PrefixLen == 16) {
        Diag(PrefixEnd, diag::err_raw_delim_too_long);
      } else {
        Diag(PrefixEnd, diag::err_invalid_char_raw_delim)
          << StringRef(BufferStart + PrefixEnd, 1);
      }
    }

    // Search for the next '"' in hopes of salvaging the lexer. Unfortunately,
    // it's possible the '"' was intended to be part of the raw string, but
    // there's not much we can do about that.
    while (true) {
      char C = BufferStart[CurOffset++];

      if (C == '"')
        break;
      if (C == 0 && CurOffset-1 == BufferSize) {
        --CurOffset;
        break;
      }
    }

    FormTokenWithChars(Result, CurOffset, tok::unknown);
    return true;
  }

  // Save prefix and move CurPtr past it
  unsigned Prefix = CurOffset;
  CurOffset += PrefixLen + 1; // skip over prefix and '('

  while (true) {
    char C = BufferStart[CurOffset++];

    if (C == ')') {
      // Check for prefix match and closing quote.
      if (strncmp(&BufferStart[CurOffset], &BufferStart[Prefix], PrefixLen) == 0 && BufferStart[CurOffset + PrefixLen] == '"') {
        CurOffset += PrefixLen + 1; // skip over prefix and '"'
        break;
      }
    } else if (C == 0 && CurOffset-1 == BufferSize) { // End of file.
      if (!isLexingRawMode())
        Diag(BufferOffset, diag::err_unterminated_raw_string)
          << StringRef(BufferStart + Prefix, PrefixLen);
      FormTokenWithChars(Result, CurOffset-1, tok::unknown);
      return true;
    }
  }

  // If we are in C++11, lex the optional ud-suffix.
  if (LangOpts.CPlusPlus)
    CurOffset = LexUDSuffix(Result, CurOffset, true);

  // Update the location of token as well as BufferPtr.
  const char *TokStart = &BufferStart[BufferOffset];
  unsigned TokLen = CurOffset - BufferOffset;
  FormTokenWithChars(Result, CurOffset, Kind);
  SetTokString(Result, StringRef(TokStart, TokLen));
  return true;
}

/// LexAngledStringLiteral - Lex the remainder of an angled string literal,
/// after having lexed the '<' character.  This is used for #include filenames.
bool Lexer::LexAngledStringLiteral(Token &Result, unsigned CurOffset) {
  // Does this string contain the \0 character?
  llvm::Optional<unsigned> NulCharacter = llvm::None;
  unsigned AfterLessPos = CurOffset;
  char C = getAndAdvanceChar(CurOffset, Result);
  while (C != '>') {
    // Skip escaped characters.  Escaped newlines will already be processed by
    // getAndAdvanceChar.
    if (C == '\\')
      C = getAndAdvanceChar(CurOffset, Result);

    if (isVerticalWhitespace(C) ||               // Newline.
        (C == 0 && (CurOffset - 1 == BufferSize))) { // End of file.
      // If the filename is unterminated, then it must just be a lone <
      // character.  Return this as such.
      FormTokenWithChars(Result, AfterLessPos, tok::less);
      return true;
    }

    if (C == 0) {
      if (isCodeCompletionPoint(CurOffset - 1)) {
        codeCompleteIncludedFile(AfterLessPos, CurOffset - 1, /*IsAngled=*/true);
        cutOffLexing();
        FormTokenWithChars(Result, CurOffset - 1, tok::unknown);
        return true;
      }
      NulCharacter = CurOffset-1;
    }
    C = getAndAdvanceChar(CurOffset, Result);
  }

  // If a nul character existed in the string, warn about it.
  if (NulCharacter.hasValue() && !isLexingRawMode())
    Diag(*NulCharacter, diag::null_in_char_or_string) << 1;

  // Update the location of token as well as BufferPtr.
  const char *TokStart = &BufferStart[BufferOffset];
  unsigned TokLen = CurOffset - BufferOffset;
  FormTokenWithChars(Result, CurOffset, tok::header_name);
  SetTokString(Result, StringRef(TokStart, TokLen));
  return true;
}

void Lexer::codeCompleteIncludedFile(unsigned PathStart,
                                     unsigned CompletionPoint,
                                     bool IsAngled) {
  // Completion only applies to the filename, after the last slash.
  StringRef PartialPath(BufferStart + PathStart, CompletionPoint - PathStart);
  llvm::StringRef SlashChars = LangOpts.MSVCCompat ? "/\\" : "/";
  auto Slash = PartialPath.find_last_of(SlashChars);
  StringRef Dir =
      (Slash == StringRef::npos) ? "" : PartialPath.take_front(Slash);
  unsigned StartOfFilename =
      (Slash == StringRef::npos) ? PathStart : PathStart + Slash + 1;
  // Code completion filter range is the filename only, up to completion point.
  PP->setCodeCompletionIdentifierInfo(&PP->getIdentifierTable().get(
      StringRef(BufferStart + StartOfFilename, CompletionPoint - StartOfFilename)));
  // We should replace the characters up to the closing quote or closest slash,
  // if any.
  while (CompletionPoint < BufferSize) {
    char Next = BufferStart[CompletionPoint + 1];
    if (Next == 0 || Next == '\r' || Next == '\n')
      break;
    ++CompletionPoint;
    if (Next == (IsAngled ? '>' : '"'))
      break;
    if (llvm::is_contained(SlashChars, Next))
      break;
  }

  PP->setCodeCompletionTokenRange(
      FileLoc.getLocWithOffset(StartOfFilename),
      FileLoc.getLocWithOffset(CompletionPoint));
  PP->CodeCompleteIncludedFile(Dir, IsAngled);
}

/// LexCharConstant - Lex the remainder of a character constant, after having
/// lexed either ' or L' or u8' or u' or U'.
bool Lexer::LexCharConstant(Token &Result, unsigned CurOffset,
                            tok::TokenKind Kind) {
  // Does this character contain the \0 character?
  llvm::Optional<unsigned> NulCharacter = llvm::None;

  if (!isLexingRawMode()) {
    if (Kind == tok::utf16_char_constant || Kind == tok::utf32_char_constant)
      Diag(BufferOffset, LangOpts.CPlusPlus
                          ? diag::warn_cxx98_compat_unicode_literal
                          : diag::warn_c99_compat_unicode_literal);
    else if (Kind == tok::utf8_char_constant)
      Diag(BufferOffset, diag::warn_cxx14_compat_u8_character_literal);
  }

  char C = getAndAdvanceChar(CurOffset, Result);
  if (C == '\'') {
    if (!isLexingRawMode() && !LangOpts.AsmPreprocessor)
      Diag(BufferOffset, diag::ext_empty_character);
    FormTokenWithChars(Result, CurOffset, tok::unknown);
    return true;
  }

  while (C != '\'') {
    // Skip escaped characters.
    if (C == '\\')
      C = getAndAdvanceChar(CurOffset, Result);

    if (C == '\n' || C == '\r' ||             // Newline.
        (C == 0 && CurOffset-1 == BufferSize && !TryExpandBuffer())) {  // End of file.
      if (!isLexingRawMode() && !LangOpts.AsmPreprocessor)
        Diag(BufferOffset, diag::ext_unterminated_char_or_string) << 0;
      FormTokenWithChars(Result, CurOffset-1, tok::unknown);
      return true;
    }

    if (C == 0) {
      if (isCodeCompletionPoint(CurOffset-1)) {
        PP->CodeCompleteNaturalLanguage();
        FormTokenWithChars(Result, CurOffset-1, tok::unknown);
        cutOffLexing();
        return true;
      }

      NulCharacter = CurOffset-1;
    }
    C = getAndAdvanceChar(CurOffset, Result);
  }

  // If we are in C++11, lex the optional ud-suffix.
  if (LangOpts.CPlusPlus)
    CurOffset = LexUDSuffix(Result, CurOffset, false);

  // If a nul character existed in the character, warn about it.
  if (NulCharacter.hasValue() && !isLexingRawMode())
    Diag(*NulCharacter, diag::null_in_char_or_string) << 0;

  // Update the location of token as well as BufferPtr.
  const char *TokStart = BufferStart + BufferOffset;
  unsigned TokLen = CurOffset - BufferOffset;
  FormTokenWithChars(Result, CurOffset, Kind);
  SetTokString(Result, StringRef(TokStart, TokLen));
  return true;
}

/// SkipWhitespace - Efficiently skip over a series of whitespace characters.
/// Update BufferPtr to point to the next non-whitespace character and return.
///
/// This method forms a token and returns true if KeepWhitespaceMode is enabled.
bool Lexer::SkipWhitespace(Token &Result, unsigned CurOffset,
                           bool &TokAtPhysicalStartOfLine) {
  // Whitespace - Skip it, then return the token after the whitespace.
  bool SawNewline = isVerticalWhitespace(BufferStart[CurOffset-1]);

  unsigned char Char = BufferStart[CurOffset];

  llvm::Optional<unsigned> lastNewLine = llvm::None;
  auto setLastNewLine = [&](unsigned Offset) {
    lastNewLine = Offset;
    if (!NewLineOffset.hasValue())
      NewLineOffset = Offset;
  };
  if (SawNewline)
    setLastNewLine(CurOffset - 1);

  // Skip consecutive spaces efficiently.
  while (true) {
    // Skip horizontal whitespace very aggressively.
    while (isHorizontalWhitespace(Char))
      Char = BufferStart[++CurOffset];

    if (Char == 0 && CurOffset == BufferSize+1 && TryExpandBuffer()) {
      --CurOffset;
      continue;
    }

    // Otherwise if we have something other than whitespace, we're done.
    if (!isVerticalWhitespace(Char))
      break;

    if (ParsingPreprocessorDirective) {
      // End of preprocessor directive line, let LexTokenInternal handle this.
      BufferOffset = CurOffset;
      return false;
    }

    // OK, but handle newline.
    if (BufferStart[CurOffset] == '\n')
      setLastNewLine(CurOffset);
    SawNewline = true;
    Char = BufferStart[++CurOffset];
  }

  // If the client wants us to return whitespace, return it now.
  if (isKeepWhitespaceMode()) {
    FormTokenWithChars(Result, CurOffset, tok::unknown);
    if (SawNewline) {
      IsAtStartOfLine = true;
      IsAtPhysicalStartOfLine = true;
    }
    // FIXME: The next token will not have LeadingSpace set.
    return true;
  }

  // If this isn't immediately after a newline, there is leading space.
  char PrevChar = BufferStart[CurOffset-1];
  bool HasLeadingSpace = !isVerticalWhitespace(PrevChar);

  Result.setFlagValue(Token::LeadingSpace, HasLeadingSpace);
  if (SawNewline) {
    Result.setFlag(Token::StartOfLine);
    TokAtPhysicalStartOfLine = true;

    if (NewLineOffset.hasValue() && lastNewLine.hasValue() && *NewLineOffset != *lastNewLine && PP) {
      if (auto *Handler = PP->getEmptylineHandler())
        Handler->HandleEmptyline(SourceRange(getSourceLocation(*NewLineOffset + 1),
                                             getSourceLocation(*lastNewLine)));
    }
  }

  BufferOffset = CurOffset;
  return false;
}

/// We have just read the // characters from input.  Skip until we find the
/// newline character that terminates the comment.  Then update BufferPtr and
/// return.
///
/// If we're in KeepCommentMode or any CommentHandler has inserted
/// some tokens, this will store the first token and return true.
bool Lexer::SkipLineComment(Token &Result, unsigned CurOffset,
                            bool &TokAtPhysicalStartOfLine) {
  // If Line comments aren't explicitly enabled for this language, emit an
  // extension warning.
  if (!LineComment) {
    if (!isLexingRawMode()) // There's no PP in raw mode, so can't emit diags.
      Diag(BufferOffset, diag::ext_line_comment);

    // Mark them enabled so we only emit one warning for this translation
    // unit.
    LineComment = true;
  }

  // Scan over the body of the comment.  The common case, when scanning, is that
  // the comment contains normal ascii characters with nothing interesting in
  // them.  As such, optimize for this case with the inner loop.
  //
  // This loop terminates with CurPtr pointing at the newline (or end of buffer)
  // character that ends the line comment.

  // C++23 [lex.phases] p1
  // Diagnose invalid UTF-8 if the corresponding warning is enabled, emitting a
  // diagnostic only once per entire ill-formed subsequence to avoid
  // emiting to many diagnostics (see http://unicode.org/review/pr-121.html).
  bool UnicodeDecodingAlreadyDiagnosed = false;

  char C;
  while (true) {
    C = BufferStart[CurOffset];
    // Skip over characters in the fast loop.
    while (true) {
      while (isASCII(C) && C != 0 &&   // Potentially EOF.
            C != '\n' && C != '\r') { // Newline or DOS-style newline.
        C = BufferStart[++CurOffset];
        UnicodeDecodingAlreadyDiagnosed = false;
      }
      if (C != 0 || CurOffset != BufferSize + 1 || !TryExpandBuffer()) break;
      C = BufferStart[CurOffset];
    }

    if (!isASCII(C)) {
      unsigned Length = llvm::getUTF8SequenceSize((const llvm::UTF8 *)&BufferStart[CurOffset],
                                    (const llvm::UTF8 *)&BufferStart[BufferSize]);
      if (Length == 0) {
        if (!UnicodeDecodingAlreadyDiagnosed && !isLexingRawMode())
          Diag(CurOffset, diag::warn_invalid_utf8_in_comment);
        UnicodeDecodingAlreadyDiagnosed = true;
        ++CurOffset;
      } else {
        UnicodeDecodingAlreadyDiagnosed = false;
        CurOffset += Length;
      }
      continue;
    }

    unsigned NextLine = CurOffset;
    if (C != 0) {
      // We found a newline, see if it's escaped.
      unsigned EscapeOffset = CurOffset-1;
      bool HasSpace = false;
      while (isHorizontalWhitespace(BufferStart[EscapeOffset])) { // Skip whitespace.
        --EscapeOffset;
        HasSpace = true;
      }

      if (BufferStart[EscapeOffset] == '\\')
        // Escaped newline.
        CurOffset = EscapeOffset;
      else if (BufferStart[EscapeOffset] == '/' && BufferStart[EscapeOffset-1] == '?' &&
               BufferStart[EscapeOffset-2] == '?' && LangOpts.Trigraphs)
        // Trigraph-escaped newline.
        CurOffset = EscapeOffset-2;
      else
        break; // This is a newline, we're done.

      // If there was space between the backslash and newline, warn about it.
      if (HasSpace && !isLexingRawMode())
        Diag(EscapeOffset, diag::backslash_newline_space);
    }

    // Otherwise, this is a hard case.  Fall back on getAndAdvanceChar to
    // properly decode the character.  Read it in raw mode to avoid emitting
    // diagnostics about things like trigraphs.  If we see an escaped newline,
    // we'll handle it below.
    unsigned OldOffset = CurOffset;
    bool OldRawMode = isLexingRawMode();
    LexingRawMode = true;
    C = getAndAdvanceChar(CurOffset, Result);
    LexingRawMode = OldRawMode;

    // If we only read only one character, then no special handling is needed.
    // We're done and can skip forward to the newline.
    if (C != 0 && CurOffset == OldOffset+1) {
      CurOffset = NextLine;
      break;
    }

    // If we read multiple characters, and one of those characters was a \r or
    // \n, then we had an escaped newline within the comment.  Emit diagnostic
    // unless the next line is also a // comment.
    if (CurOffset != OldOffset + 1 && C != '/' &&
        (CurOffset == BufferSize + 1 || BufferStart[CurOffset] != '/')) {
      for (; OldOffset != CurOffset; ++OldOffset)
        if (BufferStart[OldOffset] == '\n' || BufferStart[OldOffset] == '\r') {
          // Okay, we found a // comment that ends in a newline, if the next
          // line is also a // comment, but has spaces, don't emit a diagnostic.
          if (isWhitespace(C)) {
            unsigned ForwardOffset = CurOffset;
            while (isWhitespace(BufferStart[ForwardOffset]))  // Skip whitespace.
              ++ForwardOffset;
            if (BufferStart[ForwardOffset] == '/' && BufferStart[ForwardOffset+1] == '/')
              break;
          }

          if (!isLexingRawMode())
            Diag(OldOffset-1, diag::ext_multi_line_line_comment);
          break;
        }
    }

    if (CurOffset == BufferSize + 1) {
      if (!TryExpandBuffer()) {
        --CurOffset;
        break;
      }
      continue;
    }

    if (C == '\r' || C == '\n') {
      --CurOffset;
      break;
    }

    if (C == '\0' && isCodeCompletionPoint(CurOffset-1)) {
      PP->CodeCompleteNaturalLanguage();
      cutOffLexing();
      return false;
    }
  }

  // Found but did not consume the newline.  Notify comment handlers about the
  // comment unless we're in a #if 0 block.
  if (PP && !isLexingRawMode() &&
      PP->HandleComment(Result, SourceRange(getSourceLocation(BufferOffset),
                                            getSourceLocation(CurOffset)))) {
    BufferOffset = CurOffset;
    return true; // A token has to be returned.
  }

  // If we are returning comments as tokens, return this comment as a token.
  if (inKeepCommentMode())
    return SaveLineComment(Result, CurOffset);

  // If we are inside a preprocessor directive and we see the end of line,
  // return immediately, so that the lexer can return this as an EOD token.
  if (ParsingPreprocessorDirective || CurOffset == BufferSize) {
    BufferOffset = CurOffset;
    return false;
  }

  // Otherwise, eat the \n character.  We don't care if this is a \n\r or
  // \r\n sequence.  This is an efficiency hack (because we know the \n can't
  // contribute to another token), it isn't needed for correctness.  Note that
  // this is ok even in KeepWhitespaceMode, because we would have returned the
  /// comment above in that mode.
  NewLineOffset = CurOffset++;

  // The next returned token is at the start of the line.
  Result.setFlag(Token::StartOfLine);
  TokAtPhysicalStartOfLine = true;
  // No leading whitespace seen so far.
  Result.clearFlag(Token::LeadingSpace);
  BufferOffset = CurOffset;
  return false;
}

/// If in save-comment mode, package up this Line comment in an appropriate
/// way and return it.
bool Lexer::SaveLineComment(Token &Result, unsigned CurOffset) {
  // If we're not in a preprocessor directive, just return the // comment
  // directly.
  FormTokenWithChars(Result, CurOffset, tok::comment);

  if (!ParsingPreprocessorDirective || LexingRawMode)
    return true;

  // If this Line-style comment is in a macro definition, transmogrify it into
  // a C-style block comment.
  bool Invalid = false;
  std::string Spelling = PP->getSpelling(Result, &Invalid);
  if (Invalid)
    return true;

  assert(Spelling[0] == '/' && Spelling[1] == '/' && "Not line comment?");
  Spelling[1] = '*';   // Change prefix to "/*".
  Spelling += "*/";    // add suffix.

  Result.setKind(tok::comment);
  PP->CreateString(Spelling, Result,
                   Result.getLocation(), Result.getLocation());
  return true;
}

/// isBlockCommentEndOfEscapedNewLine - Return true if the specified newline
/// character (either \\n or \\r) is part of an escaped newline sequence.  Issue
/// a diagnostic if so.  We know that the newline is inside of a block comment.
static bool isEndOfBlockCommentWithEscapedNewLine(const char* CurPtr, Lexer *L,
                                                  bool Trigraphs) {
  assert(CurPtr[0] == '\n' || CurPtr[0] == '\r');

  // Position of the first trigraph in the ending sequence.
  const char *TrigraphPos = nullptr;
  // Position of the first whitespace after a '\' in the ending sequence.
  const char *SpacePos = nullptr;

  while (true) {
    // Back up off the newline.
    --CurPtr;

    // If this is a two-character newline sequence, skip the other character.
    if (CurPtr[0] == '\n' || CurPtr[0] == '\r') {
      // \n\n or \r\r -> not escaped newline.
      if (CurPtr[0] == CurPtr[1])
        return false;
      // \n\r or \r\n -> skip the newline.
      --CurPtr;
    }

    // If we have horizontal whitespace, skip over it.  We allow whitespace
    // between the slash and newline.
    while (isHorizontalWhitespace(*CurPtr) || *CurPtr == 0) {
      SpacePos = CurPtr;
      --CurPtr;
    }

    // If we have a slash, this is an escaped newline.
    if (*CurPtr == '\\') {
      --CurPtr;
    } else if (CurPtr[0] == '/' && CurPtr[-1] == '?' && CurPtr[-2] == '?') {
      // This is a trigraph encoding of a slash.
      TrigraphPos = CurPtr - 2;
      CurPtr -= 3;
    } else {
      return false;
    }

    // If the character preceding the escaped newline is a '*', then after line
    // splicing we have a '*/' ending the comment.
    if (*CurPtr == '*')
      break;

    if (*CurPtr != '\n' && *CurPtr != '\r')
      return false;
  }

  if (TrigraphPos) {
    // If no trigraphs are enabled, warn that we ignored this trigraph and
    // ignore this * character.
    if (!Trigraphs) {
      if (!L->isLexingRawMode())
        L->Diag(TrigraphPos - L->getBuffer().data(), diag::trigraph_ignored_block_comment);
      return false;
    }
    if (!L->isLexingRawMode())
      L->Diag(TrigraphPos- L->getBuffer().data(), diag::trigraph_ends_block_comment);
  }

  // Warn about having an escaped newline between the */ characters.
  if (!L->isLexingRawMode())
    L->Diag(CurPtr+ 1- L->getBuffer().data(), diag::escaped_newline_block_comment_end);

  // If there was space between the backslash and newline, warn about it.
  if (SpacePos && !L->isLexingRawMode())
    L->Diag(SpacePos- L->getBuffer().data(), diag::backslash_newline_space);

  return true;
}

#ifdef __SSE2__
#include <emmintrin.h>
#elif __ALTIVEC__
#include <altivec.h>
#undef bool
#endif

/// We have just read from input the / and * characters that started a comment.
/// Read until we find the * and / characters that terminate the comment.
/// Note that we don't bother decoding trigraphs or escaped newlines in block
/// comments, because they cannot cause the comment to end.  The only thing
/// that can happen is the comment could end with an escaped newline between
/// the terminating * and /.
///
/// If we're in KeepCommentMode or any CommentHandler has inserted
/// some tokens, this will store the first token and return true.
bool Lexer::SkipBlockComment(Token &Result, unsigned CurOffset,
                             bool &TokAtPhysicalStartOfLine) {
  // Scan one character past where we should, looking for a '/' character.  Once
  // we find it, check to see if it was preceded by a *.  This common
  // optimization helps people who like to put a lot of * characters in their
  // comments.

  // The first character we get with newlines and trigraphs skipped to handle
  // the degenerate /*/ case below correctly if the * has an escaped newline
  // after it.
  unsigned CharSize;
  unsigned char C = getCharAndSize(CurOffset, CharSize);
  CurOffset += CharSize;
  if (C == 0 && CurOffset == BufferSize+1 && !TryExpandBuffer()) {
    if (!isLexingRawMode())
      Diag(BufferOffset, diag::err_unterminated_block_comment);
    --CurOffset;

    // KeepWhitespaceMode should return this broken comment as a token.  Since
    // it isn't a well formed comment, just return it as an 'unknown' token.
    if (isKeepWhitespaceMode()) {
      FormTokenWithChars(Result, CurOffset, tok::unknown);
      return true;
    }

    BufferOffset = CurOffset;
    return false;
  }

  // Check to see if the first character after the '/*' is another /.  If so,
  // then this slash does not end the block comment, it is part of it.
  if (C == '/')
    C = BufferStart[CurOffset++];

  // C++23 [lex.phases] p1
  // Diagnose invalid UTF-8 if the corresponding warning is enabled, emitting a
  // diagnostic only once per entire ill-formed subsequence to avoid
  // emiting to many diagnostics (see http://unicode.org/review/pr-121.html).
  bool UnicodeDecodingAlreadyDiagnosed = false;

  while (true) {
    if (CurOffset + 24 >= BufferSize) {
      TryExpandBuffer();
    }
    // Skip over all non-interesting characters until we find end of buffer or a
    // (probably ending) '/' character.
    if (CurOffset + 24 < BufferSize &&
        // If there is a code-completion point avoid the fast scan because it
        // doesn't check for '\0'.
        !(PP && PP->getCodeCompletionFileLoc() == FileLoc)) {
      // While not aligned to a 16-byte boundary.
      while (C != '/' && (intptr_t)(BufferStart + CurOffset) % 16 != 0) {
        if (!isASCII(C))
          goto MultiByteUTF8;
        C = BufferStart[CurOffset++];
      }
      if (C == '/') goto FoundSlash;

#ifdef __SSE2__
      __m128i Slashes = _mm_set1_epi8('/');
      while (CurOffset + 16 < BufferSize) {
        int Mask = _mm_movemask_epi8(*(const __m128i *)(BufferStart + CurOffset));
        if (LLVM_UNLIKELY(Mask != 0)) {
          goto MultiByteUTF8;
        }
        // look for slashes
        int cmp = _mm_movemask_epi8(_mm_cmpeq_epi8(*(const __m128i*)(BufferStart + CurOffset),
                                    Slashes));
        if (cmp != 0) {
          // Adjust the pointer to point directly after the first slash. It's
          // not necessary to set C here, it will be overwritten at the end of
          // the outer loop.
          CurOffset += llvm::countTrailingZeros<unsigned>(cmp) + 1;
          goto FoundSlash;
        }
        CurOffset += 16;
      }
#elif __ALTIVEC__
      __vector unsigned char LongUTF = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                                        0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                                        0x80, 0x80, 0x80, 0x80};
      __vector unsigned char Slashes = {
        '/', '/', '/', '/',  '/', '/', '/', '/',
        '/', '/', '/', '/',  '/', '/', '/', '/'
      };
      while (CurPtr + 16 < BufferEnd) {
        if (LLVM_UNLIKELY(
                vec_any_ge(*(const __vector unsigned char *)CurPtr, LongUTF)))
          goto MultiByteUTF8;
        if (vec_any_eq(*(const __vector unsigned char *)CurPtr, Slashes)) {
          break;
        }
        CurPtr += 16;
      }

#else
      while (CurOffset + 16 < BufferSize) {
        bool HasNonASCII = false;
        for (unsigned I = 0; I < 16; ++I)
          HasNonASCII |= !isASCII(BufferStart[CurOffset+I]);

        if (LLVM_UNLIKELY(HasNonASCII))
          goto MultiByteUTF8;

        bool HasSlash = false;
        for (unsigned I = 0; I < 16; ++I)
          HasSlash |= BufferStart[CurOffset+I] == '/';
        if (HasSlash)
          break;
        CurOffset += 16;
      }
#endif

      // It has to be one of the bytes scanned, increment to it and read one.
      C = BufferStart[CurOffset++];
    }

    // Loop to scan the remainder, warning on invalid UTF-8
    // if the corresponding warning is enabled, emitting a diagnostic only once
    // per sequence that cannot be decoded.
    while (C != '/' && C != '\0') {
      if (isASCII(C)) {
        UnicodeDecodingAlreadyDiagnosed = false;
        C = BufferStart[CurOffset++];
        continue;
      }
    MultiByteUTF8:
      // CurPtr is 1 code unit past C, so to decode
      // the codepoint, we need to read from the previous position.
      unsigned Length = llvm::getUTF8SequenceSize(
          (const llvm::UTF8 *)(BufferStart + CurOffset) - 1, (const llvm::UTF8 *)(BufferStart + BufferSize));
      if (Length == 0) {
        if (!UnicodeDecodingAlreadyDiagnosed && !isLexingRawMode())
          Diag(CurOffset - 1, diag::warn_invalid_utf8_in_comment);
        UnicodeDecodingAlreadyDiagnosed = true;
      } else {
        UnicodeDecodingAlreadyDiagnosed = false;
        CurOffset += Length - 1;
      }
      C = BufferStart[CurOffset++];
    }

    if (C == '/') {
  FoundSlash:
      if (BufferStart[CurOffset-2] == '*')  // We found the final */.  We're done!
        break;

      if ((BufferStart[CurOffset-2] == '\n' || BufferStart[CurOffset-2] == '\r')) {
        if (isEndOfBlockCommentWithEscapedNewLine(&BufferStart[CurOffset - 2], this,
                                                  LangOpts.Trigraphs)) {
          // We found the final */, though it had an escaped newline between the
          // * and /.  We're done!
          break;
        }
      }
      if (BufferStart[CurOffset] == '*' && BufferStart[CurOffset+1] != '/') {
        // If this is a /* inside of the comment, emit a warning.  Don't do this
        // if this is a /*/, which will end the comment.  This misses cases with
        // embedded escaped newlines, but oh well.
        if (!isLexingRawMode())
          Diag(CurOffset-1, diag::warn_nested_block_comment);
      }
    } else if (C == 0 && CurOffset == BufferSize+1 && TryExpandBuffer()) {
      --CurOffset;
      continue;
    } else if (C == 0 && CurOffset == BufferSize+1) {
      if (!isLexingRawMode())
        Diag(BufferOffset, diag::err_unterminated_block_comment);
      // Note: the user probably forgot a */.  We could continue immediately
      // after the /*, but this would involve lexing a lot of what really is the
      // comment, which surely would confuse the parser.
      --CurOffset;

      // KeepWhitespaceMode should return this broken comment as a token.  Since
      // it isn't a well formed comment, just return it as an 'unknown' token.
      if (isKeepWhitespaceMode()) {
        FormTokenWithChars(Result, CurOffset, tok::unknown);
        return true;
      }

      BufferOffset = CurOffset;
      return false;
    } else if (C == '\0' && isCodeCompletionPoint(CurOffset-1)) {
      PP->CodeCompleteNaturalLanguage();
      cutOffLexing();
      return false;
    }

    C = BufferStart[CurOffset++];
  }

  // Notify comment handlers about the comment unless we're in a #if 0 block.
  if (PP && !isLexingRawMode() &&
      PP->HandleComment(Result, SourceRange(getSourceLocation(BufferOffset),
                                            getSourceLocation(CurOffset)))) {
    BufferOffset = CurOffset;
    return true; // A token has to be returned.
  }

  // If we are returning comments as tokens, return this comment as a token.
  if (inKeepCommentMode()) {
    FormTokenWithChars(Result, CurOffset, tok::comment);
    return true;
  }

  // It is common for the tokens immediately after a /**/ comment to be
  // whitespace.  Instead of going through the big switch, handle it
  // efficiently now.  This is safe even in KeepWhitespaceMode because we would
  // have already returned above with the comment as a token.
  if (isHorizontalWhitespace(BufferStart[CurOffset])) {
    SkipWhitespace(Result, CurOffset+1, TokAtPhysicalStartOfLine);
    return false;
  }

  // Otherwise, just return so that the next character will be lexed as a token.
  BufferOffset = CurOffset;
  Result.setFlag(Token::LeadingSpace);
  return false;
}

//===----------------------------------------------------------------------===//
// Primary Lexing Entry Points
//===----------------------------------------------------------------------===//

/// ReadToEndOfLine - Read the rest of the current preprocessor line as an
/// uninterpreted string.  This switches the lexer out of directive mode.
void Lexer::ReadToEndOfLine(SmallVectorImpl<char> *Result) {
  assert(ParsingPreprocessorDirective && ParsingFilename == false &&
         "Must be in a preprocessing directive!");
  Token Tmp;
  Tmp.startToken();

  // CurPtr - Cache BufferPtr in an automatic variable.
  unsigned CurOffset = BufferOffset;
  while (true) {
    char Char = getAndAdvanceChar(CurOffset, Tmp);
    switch (Char) {
    default:
      if (Result)
        Result->push_back(Char);
      break;
    case 0:  // Null.
      // Found end of file?
      if (CurOffset-1 != BufferSize && !TryExpandBuffer()) {
        if (isCodeCompletionPoint(CurOffset-1)) {
          PP->CodeCompleteNaturalLanguage();
          cutOffLexing();
          return;
        }

        // Nope, normal character, continue.
        if (Result)
          Result->push_back(Char);
        break;
      }
      // FALL THROUGH.
      [[fallthrough]];
    case '\r':
    case '\n':
      // Okay, we found the end of the line. First, back up past the \0, \r, \n.
      assert(BufferStart[CurOffset-1] == Char && "Trigraphs for newline?");
      BufferOffset = CurOffset-1;

      // Next, lex the character, which should handle the EOD transition.
      Lex(Tmp);
      if (Tmp.is(tok::code_completion)) {
        if (PP)
          PP->CodeCompleteNaturalLanguage();
        Lex(Tmp);
      }
      assert(Tmp.is(tok::eod) && "Unexpected token!");

      // Finally, we're done;
      return;
    }
  }
}

/// LexEndOfFile - CurPtr points to the end of this file.  Handle this
/// condition, reporting diagnostics and handling other edge cases as required.
/// This returns true if Result contains a token, false if PP.Lex should be
/// called again.
bool Lexer::LexEndOfFile(Token &Result, unsigned CurOffset) {
  // If we hit the end of the file while parsing a preprocessor directive,
  // end the preprocessor directive first.  The next token returned will
  // then be the end of file.
  if (ParsingPreprocessorDirective) {
    // Done parsing the "line".
    ParsingPreprocessorDirective = false;
    // Update the location of token as well as BufferPtr.
    FormTokenWithChars(Result, CurOffset, tok::eod);

    // Restore comment saving mode, in case it was disabled for directive.
    if (PP)
      resetExtendedTokenMode();
    return true;  // Have a token.
  }

  // If we are in raw mode, return this event as an EOF token.  Let the caller
  // that put us in raw mode handle the event.
  if (isLexingRawMode()) {
    Result.startToken();
    BufferOffset = BufferSize;
    FormTokenWithChars(Result, BufferSize, tok::eof);
    return true;
  }

  if (PP->isRecordingPreamble() && PP->isInPrimaryFile()) {
    PP->setRecordedPreambleConditionalStack(ConditionalStack);
    // If the preamble cuts off the end of a header guard, consider it guarded.
    // The guard is valid for the preamble content itself, and for tools the
    // most useful answer is "yes, this file has a header guard".
    if (!ConditionalStack.empty())
      MIOpt.ExitTopLevelConditional();
    ConditionalStack.clear();
  }

  // Issue diagnostics for unterminated #if and missing newline.

  // If we are in a #if directive, emit an error.
  while (!ConditionalStack.empty()) {
    if (PP->getCodeCompletionFileLoc() != FileLoc)
      PP->Diag(ConditionalStack.back().IfLoc,
               diag::err_pp_unterminated_conditional);
    ConditionalStack.pop_back();
  }

  // C99 5.1.1.2p2: If the file is non-empty and didn't end in a newline, issue
  // a pedwarn.
  if (CurOffset != 0 && (BufferStart[CurOffset-1] != '\n' && BufferStart[CurOffset-1] != '\r')) {
    DiagnosticsEngine &Diags = PP->getDiagnostics();
    SourceLocation EndLoc = getSourceLocation(BufferSize);
    unsigned DiagID;

    if (LangOpts.CPlusPlus11) {
      // C++11 [lex.phases] 2.2 p2
      // Prefer the C++98 pedantic compatibility warning over the generic,
      // non-extension, user-requested "missing newline at EOF" warning.
      if (!Diags.isIgnored(diag::warn_cxx98_compat_no_newline_eof, EndLoc)) {
        DiagID = diag::warn_cxx98_compat_no_newline_eof;
      } else {
        DiagID = diag::warn_no_newline_eof;
      }
    } else {
      DiagID = diag::ext_no_newline_eof;
    }

    Diag(BufferSize, DiagID)
      << FixItHint::CreateInsertion(EndLoc, "\n");
  }

  BufferOffset = CurOffset;

  // Finally, let the preprocessor handle this.
  return PP->HandleEndOfFile(Result, isPragmaLexer());
}

/// isNextPPTokenLParen - Return 1 if the next unexpanded token lexed from
/// the specified lexer will return a tok::l_paren token, 0 if it is something
/// else and 2 if there are no more tokens in the buffer controlled by the
/// lexer.
unsigned Lexer::isNextPPTokenLParen() {
  assert(!LexingRawMode && "How can we expand a macro from a skipping buffer?");

  if (isDependencyDirectivesLexer()) {
    if (NextDepDirectiveTokenIndex == DepDirectives.front().Tokens.size())
      return 2;
    return DepDirectives.front().Tokens[NextDepDirectiveTokenIndex].is(
        tok::l_paren);
  }

  // Switch to 'skipping' mode.  This will ensure that we can lex a token
  // without emitting diagnostics, disables macro expansion, and will cause EOF
  // to return an EOF token instead of popping the include stack.
  LexingRawMode = true;

  // Save state that can be changed while lexing so that we can restore it.
  unsigned TmpBufferOffset = BufferOffset;
  bool inPPDirectiveMode = ParsingPreprocessorDirective;
  bool atStartOfLine = IsAtStartOfLine;
  bool atPhysicalStartOfLine = IsAtPhysicalStartOfLine;
  bool leadingSpace = HasLeadingSpace;

  Token Tok;
  Lex(Tok);

  // Restore state that may have changed.
  BufferOffset = TmpBufferOffset;
  ParsingPreprocessorDirective = inPPDirectiveMode;
  HasLeadingSpace = leadingSpace;
  IsAtStartOfLine = atStartOfLine;
  IsAtPhysicalStartOfLine = atPhysicalStartOfLine;

  // Restore the lexer back to non-skipping mode.
  LexingRawMode = false;

  if (Tok.is(tok::eof))
    return 2;
  return Tok.is(tok::l_paren);
}

/// Find the end of a version control conflict marker.
static const char *FindConflictEnd(const char* CurPtr, const char *BufferEnd,
                                   ConflictMarkerKind CMK) {
  const char *Terminator = CMK == CMK_Perforce ? "<<<<\n" : ">>>>>>>";
  size_t TermLen = CMK == CMK_Perforce ? 5 : 7;
  auto RestOfBuffer = StringRef(CurPtr, BufferEnd - CurPtr).substr(TermLen);
  size_t Pos = RestOfBuffer.find(Terminator);
  while (Pos != StringRef::npos) {
    // Must occur at start of line.
    if (Pos == 0 ||
        (RestOfBuffer[Pos - 1] != '\r' && RestOfBuffer[Pos - 1] != '\n')) {
      RestOfBuffer = RestOfBuffer.substr(Pos+TermLen);
      Pos = RestOfBuffer.find(Terminator);
      continue;
    }
    return RestOfBuffer.data()+Pos;
  }
  return nullptr;
}

/// IsStartOfConflictMarker - If the specified pointer is the start of a version
/// control conflict marker like '<<<<<<<', recognize it as such, emit an error
/// and recover nicely.  This returns true if it is a conflict marker and false
/// if not.
bool Lexer::IsStartOfConflictMarker(unsigned CurOffset) {
  // Only a conflict marker if it starts at the beginning of a line.
  if (CurOffset != 0 &&
      BufferStart[CurOffset-1] != '\n' && BufferStart[CurOffset-1] != '\r')
    return false;

  // Check to see if we have <<<<<<< or >>>>.
  if (!StringRef(BufferStart+CurOffset, BufferSize - CurOffset).startswith("<<<<<<<") &&
      !StringRef(BufferStart+CurOffset, BufferSize - CurOffset).startswith(">>>> "))
    return false;

  // If we have a situation where we don't care about conflict markers, ignore
  // it.
  if (CurrentConflictMarkerState || isLexingRawMode())
    return false;

  ConflictMarkerKind Kind = BufferStart[CurOffset] == '<' ? CMK_Normal : CMK_Perforce;

  // Check to see if there is an ending marker somewhere in the buffer at the
  // start of a line to terminate this conflict marker.
  if (FindConflictEnd(&BufferStart[CurOffset], &BufferStart[BufferSize], Kind)) {
    // We found a match.  We are really in a conflict marker.
    // Diagnose this, and ignore to the end of line.
    Diag(CurOffset, diag::err_conflict_marker);
    CurrentConflictMarkerState = Kind;

    // Skip ahead to the end of line.  We know this exists because the
    // end-of-conflict marker starts with \r or \n.
    while (BufferStart[CurOffset] != '\r' && BufferStart[CurOffset] != '\n') {
      assert(CurOffset != BufferSize && "Didn't find end of line");
      ++CurOffset;
    }
    BufferOffset = CurOffset;
    return true;
  }

  // No end of conflict marker found.
  return false;
}

/// HandleEndOfConflictMarker - If this is a '====' or '||||' or '>>>>', or if
/// it is '<<<<' and the conflict marker started with a '>>>>' marker, then it
/// is the end of a conflict marker.  Handle it by ignoring up until the end of
/// the line.  This returns true if it is a conflict marker and false if not.
bool Lexer::HandleEndOfConflictMarker(unsigned CurOffset) {
  // Only a conflict marker if it starts at the beginning of a line.
  if (CurOffset != 0 &&
      BufferStart[CurOffset-1] != '\n' && BufferStart[CurOffset-1] != '\r')
    return false;

  // If we have a situation where we don't care about conflict markers, ignore
  // it.
  if (!CurrentConflictMarkerState || isLexingRawMode())
    return false;

  // Check to see if we have the marker (4 characters in a row).
  for (unsigned i = 1; i != 4; ++i)
    if (BufferStart[CurOffset + i] != BufferStart[CurOffset])
      return false;

  // If we do have it, search for the end of the conflict marker.  This could
  // fail if it got skipped with a '#if 0' or something.  Note that CurPtr might
  // be the end of conflict marker.
  if (const char *End = FindConflictEnd(BufferStart + CurOffset, BufferStart + BufferSize,
                                        CurrentConflictMarkerState)) {
    CurOffset = End - BufferStart;

    // Skip ahead to the end of line.
    while (CurOffset != BufferSize && BufferStart[CurOffset] != '\r' && BufferStart[CurOffset] != '\n')
      ++CurOffset;

    BufferOffset = CurOffset;

    // No longer in the conflict marker.
    CurrentConflictMarkerState = CMK_None;
    return true;
  }

  return false;
}

static const char *findPlaceholderEnd(const char* CurPtr,
                                      const char *BufferEnd) {
  if (CurPtr == BufferEnd)
    return nullptr;
  BufferEnd -= 1; // Scan until the second last character.
  for (; CurPtr != BufferEnd; ++CurPtr) {
    if (CurPtr[0] == '#' && CurPtr[1] == '>')
      return CurPtr + 2;
  }
  return nullptr;
}

void Lexer::SetTokString(Token& Tok, StringRef Str) {
  const char* DestPtr = Str.data();
  if (GrowBuffer) {
    assert(PP);
    PP->CreateString(Str, Tok);
  }

  if (Tok.is(tok::raw_identifier))
    Tok.setRawIdentifierData(DestPtr);
  else
    Tok.setLiteralData(DestPtr);
}

bool Lexer::lexEditorPlaceholder(Token &Result, unsigned CurOffset) {
  assert(BufferStart[CurOffset-1] == '<' && BufferStart[CurOffset] == '#' && "Not a placeholder!");
  if (!PP || !PP->getPreprocessorOpts().LexEditorPlaceholders || LexingRawMode)
    return false;
  if (CurOffset + 1 == BufferSize) 
    TryExpandBuffer();
  const char *End = findPlaceholderEnd(BufferStart + CurOffset + 1, BufferStart + BufferSize);
  if (!End)
    return false;
  const char *Start = BufferStart + CurOffset - 1;
  unsigned TokLen = End-Start;
  if (!LangOpts.AllowEditorPlaceholders)
    Diag(CurOffset - 1, diag::err_placeholder_in_source);
  Result.startToken();
  FormTokenWithChars(Result, End - BufferStart, tok::raw_identifier);
  SetTokString(Result, StringRef(Start, TokLen));
  PP->LookUpIdentifierInfo(Result);
  Result.setFlag(Token::IsEditorPlaceholder);
  BufferOffset = End - BufferStart;
  return true;
}

bool Lexer::isCodeCompletionPoint(unsigned CurOffset) const {
  if (PP && PP->isCodeCompletionEnabled()) {
    SourceLocation Loc = FileLoc.getLocWithOffset(CurOffset);
    return Loc == PP->getCodeCompletionLoc();
  }

  return false;
}

llvm::Optional<uint32_t> Lexer::tryReadNumericUCN(unsigned &StartOffset,
                                                  unsigned SlashLoc,
                                                  Token *Result) {
  unsigned CharSize;
  char Kind = getCharAndSize(StartOffset, CharSize);
  assert((Kind == 'u' || Kind == 'U') && "expected a UCN");

  unsigned NumHexDigits;
  if (Kind == 'u')
    NumHexDigits = 4;
  else if (Kind == 'U')
    NumHexDigits = 8;

  bool Delimited = false;
  bool FoundEndDelimiter = false;
  unsigned Count = 0;
  bool Diagnose = Result && !isLexingRawMode();

  if (!LangOpts.CPlusPlus && !LangOpts.C99) {
    if (Diagnose)
      Diag(SlashLoc, diag::warn_ucn_not_valid_in_c89);
    return llvm::None;
  }

  unsigned CurOffset = StartOffset + CharSize;
  unsigned KindLoc = CurOffset-1;

  uint32_t CodePoint = 0;
  while (Count != NumHexDigits || Delimited) {
    char C = getCharAndSize(CurOffset, CharSize);
    if (!Delimited && C == '{') {
      Delimited = true;
      CurOffset += CharSize;
      continue;
    }

    if (Delimited && C == '}') {
      CurOffset += CharSize;
      FoundEndDelimiter = true;
      break;
    }

    unsigned Value = llvm::hexDigitValue(C);
    if (Value == -1U) {
      if (!Delimited)
        break;
      if (Diagnose)
        Diag(BufferOffset, diag::warn_delimited_ucn_incomplete)
            << StringRef(BufferStart+KindLoc, 1);
      return llvm::None;
    }

    if (CodePoint & 0xF000'0000) {
      if (Diagnose)
        Diag(KindLoc, diag::err_escape_too_large) << 0;
      return llvm::None;
    }

    CodePoint <<= 4;
    CodePoint |= Value;
    CurOffset += CharSize;
    Count++;
  }

  if (Count == 0) {
    if (Diagnose)
      Diag(StartOffset, FoundEndDelimiter ? diag::warn_delimited_ucn_empty
                                       : diag::warn_ucn_escape_no_digits)
          << StringRef(BufferStart+KindLoc, 1);
    return llvm::None;
  }

  if (Delimited && Kind == 'U') {
    if (Diagnose)
      Diag(StartOffset, diag::err_hex_escape_no_digits) << StringRef(BufferStart+KindLoc, 1);
    return llvm::None;
  }

  if (!Delimited && Count != NumHexDigits) {
    if (Diagnose) {
      Diag(BufferOffset, diag::warn_ucn_escape_incomplete);
      // If the user wrote \U1234, suggest a fixit to \u.
      if (Count == 4 && NumHexDigits == 8) {
        CharSourceRange URange = makeCharRange(*this, KindLoc, KindLoc + 1);
        Diag(KindLoc, diag::note_ucn_four_not_eight)
            << FixItHint::CreateReplacement(URange, "u");
      }
    }
    return llvm::None;
  }

  if (Delimited && PP) {
    Diag(BufferOffset, PP->getLangOpts().CPlusPlus2b
                        ? diag::warn_cxx2b_delimited_escape_sequence
                        : diag::ext_delimited_escape_sequence)
        << /*delimited*/ 0 << (PP->getLangOpts().CPlusPlus ? 1 : 0);
  }

  if (Result) {
    Result->setFlag(Token::HasUCN);
    if (CurOffset - StartOffset == (ptrdiff_t)(Count + 2 + (Delimited ? 2 : 0)))
      StartOffset = CurOffset;
    else
      while (StartOffset != CurOffset)
        (void)getAndAdvanceChar(StartOffset, *Result);
  } else {
    StartOffset = CurOffset;
  }
  return CodePoint;
}

llvm::Optional<uint32_t> Lexer::tryReadNamedUCN(unsigned &StartOffset,
                                                Token *Result) {
  unsigned CharSize;
  bool Diagnose = Result && !isLexingRawMode();

  char C = getCharAndSize(StartOffset, CharSize);
  assert(C == 'N' && "expected \\N{...}");

  unsigned CurOffset = StartOffset + CharSize;
  unsigned KindLoc = CurOffset - 1;

  C = getCharAndSize(CurOffset, CharSize);
  if (C != '{') {
    if (Diagnose)
      Diag(StartOffset, diag::warn_ucn_escape_incomplete);
    return llvm::None;
  }
  CurOffset += CharSize;
  unsigned StartName = CurOffset;
  bool FoundEndDelimiter = false;
  llvm::SmallVector<char, 30> Buffer;
  while (C) {
    C = getCharAndSize(CurOffset, CharSize);
    CurOffset += CharSize;
    if (C == '}') {
      FoundEndDelimiter = true;
      break;
    }

    if (!isAlphanumeric(C) && C != '_' && C != '-' && C != ' ')
      break;
    Buffer.push_back(C);
  }

  if (!FoundEndDelimiter || Buffer.empty()) {
    if (Diagnose)
      Diag(StartOffset, FoundEndDelimiter ? diag::warn_delimited_ucn_empty
                                       : diag::warn_delimited_ucn_incomplete)
          << StringRef(BufferStart+KindLoc, 1);
    return llvm::None;
  }

  StringRef Name(Buffer.data(), Buffer.size());
  llvm::Optional<char32_t> Res =
      llvm::sys::unicode::nameToCodepointStrict(Name);
  llvm::Optional<llvm::sys::unicode::LooseMatchingResult> LooseMatch;
  if (!Res) {
    if (!isLexingRawMode()) {
      Diag(StartOffset, diag::err_invalid_ucn_name)
          << StringRef(Buffer.data(), Buffer.size());
      LooseMatch = llvm::sys::unicode::nameToCodepointLooseMatching(Name);
      if (LooseMatch) {
        Diag(StartName, diag::note_invalid_ucn_name_loose_matching)
            << FixItHint::CreateReplacement(
                   makeCharRange(*this, StartName, CurOffset - CharSize),
                   LooseMatch->Name);
      }
    }
    // When finding a match using Unicode loose matching rules
    // recover after having emitted a diagnostic.
    if (!LooseMatch)
      return llvm::None;
    // We do not offer misspelled character names suggestions here
    // as the set of what would be a valid suggestion depends on context,
    // and we should not make invalid suggestions.
  }

  if (Diagnose && PP && !LooseMatch)
    Diag(BufferOffset, PP->getLangOpts().CPlusPlus2b
                        ? diag::warn_cxx2b_delimited_escape_sequence
                        : diag::ext_delimited_escape_sequence)
        << /*named*/ 1 << (PP->getLangOpts().CPlusPlus ? 1 : 0);

  if (LooseMatch)
    Res = LooseMatch->CodePoint;

  if (Result) {
    Result->setFlag(Token::HasUCN);
    if (CurOffset - StartOffset == (ptrdiff_t)(Buffer.size() + 4))
      StartOffset = CurOffset;
    else
      while (StartOffset != CurOffset)
        (void)getAndAdvanceChar(StartOffset, *Result);
  } else {
    StartOffset = CurOffset;
  }
  return *Res;
}

uint32_t Lexer::tryReadUCN(unsigned &StartOffset, unsigned SlashLoc,
                           Token *Result) {

  unsigned CharSize;
  llvm::Optional<uint32_t> CodePointOpt;
  char Kind = getCharAndSize(StartOffset, CharSize);
  if (Kind == 'u' || Kind == 'U')
    CodePointOpt = tryReadNumericUCN(StartOffset, SlashLoc, Result);
  else if (Kind == 'N')
    CodePointOpt = tryReadNamedUCN(StartOffset, Result);

  if (!CodePointOpt)
    return 0;

  uint32_t CodePoint = *CodePointOpt;

  // Don't apply C family restrictions to UCNs in assembly mode
  if (LangOpts.AsmPreprocessor)
    return CodePoint;

  // C99 6.4.3p2: A universal character name shall not specify a character whose
  //   short identifier is less than 00A0 other than 0024 ($), 0040 (@), or
  //   0060 (`), nor one in the range D800 through DFFF inclusive.)
  // C++11 [lex.charset]p2: If the hexadecimal value for a
  //   universal-character-name corresponds to a surrogate code point (in the
  //   range 0xD800-0xDFFF, inclusive), the program is ill-formed. Additionally,
  //   if the hexadecimal value for a universal-character-name outside the
  //   c-char-sequence, s-char-sequence, or r-char-sequence of a character or
  //   string literal corresponds to a control character (in either of the
  //   ranges 0x00-0x1F or 0x7F-0x9F, both inclusive) or to a character in the
  //   basic source character set, the program is ill-formed.
  if (CodePoint < 0xA0) {
    if (CodePoint == 0x24 || CodePoint == 0x40 || CodePoint == 0x60)
      return CodePoint;

    // We don't use isLexingRawMode() here because we need to warn about bad
    // UCNs even when skipping preprocessing tokens in a #if block.
    if (Result && PP) {
      if (CodePoint < 0x20 || CodePoint >= 0x7F)
        Diag(BufferOffset, diag::err_ucn_control_character);
      else {
        char C = static_cast<char>(CodePoint);
        Diag(BufferOffset, diag::err_ucn_escape_basic_scs) << StringRef(&C, 1);
      }
    }

    return 0;
  } else if (CodePoint >= 0xD800 && CodePoint <= 0xDFFF) {
    // C++03 allows UCNs representing surrogate characters. C99 and C++11 don't.
    // We don't use isLexingRawMode() here because we need to diagnose bad
    // UCNs even when skipping preprocessing tokens in a #if block.
    if (Result && PP) {
      if (LangOpts.CPlusPlus && !LangOpts.CPlusPlus11)
        Diag(BufferOffset, diag::warn_ucn_escape_surrogate);
      else
        Diag(BufferOffset, diag::err_ucn_escape_invalid);
    }
    return 0;
  }

  return CodePoint;
}

bool Lexer::CheckUnicodeWhitespace(Token &Result, uint32_t C,
                                   unsigned CurOffset) {
  if (!isLexingRawMode() && !PP->isPreprocessedOutput() &&
      isUnicodeWhitespace(C)) {
    Diag(BufferOffset, diag::ext_unicode_whitespace)
      << makeCharRange(*this, BufferOffset, CurOffset);

    Result.setFlag(Token::LeadingSpace);
    return true;
  }
  return false;
}

void Lexer::PropagateLineStartLeadingSpaceInfo(Token &Result) {
  IsAtStartOfLine = Result.isAtStartOfLine();
  HasLeadingSpace = Result.hasLeadingSpace();
  HasLeadingEmptyMacro = Result.hasLeadingEmptyMacro();
  // Note that this doesn't affect IsAtPhysicalStartOfLine.
}

bool Lexer::Lex(Token &Result) {
  assert(!isDependencyDirectivesLexer());

  // Start a new token.
  Result.startToken();

  // Set up misc whitespace flags for LexTokenInternal.
  if (IsAtStartOfLine) {
    Result.setFlag(Token::StartOfLine);
    IsAtStartOfLine = false;
  }

  if (HasLeadingSpace) {
    Result.setFlag(Token::LeadingSpace);
    HasLeadingSpace = false;
  }

  if (HasLeadingEmptyMacro) {
    Result.setFlag(Token::LeadingEmptyMacro);
    HasLeadingEmptyMacro = false;
  }

  bool atPhysicalStartOfLine = IsAtPhysicalStartOfLine;
  IsAtPhysicalStartOfLine = false;
  bool isRawLex = isLexingRawMode();
  (void) isRawLex;
  bool returnedToken = LexTokenInternal(Result, atPhysicalStartOfLine);
  // (After the LexTokenInternal call, the lexer might be destroyed.)
  assert((returnedToken || !isRawLex) && "Raw lex must succeed");
  return returnedToken;
}

/// LexTokenInternal - This implements a simple C family lexer.  It is an
/// extremely performance critical piece of code.  This assumes that the buffer
/// has a null character at the end of the file.  This returns a preprocessing
/// token, not a normal token, as such, it is an internal interface.  It assumes
/// that the Flags of result have been cleared before calling this.
bool Lexer::LexTokenInternal(Token &Result, bool TokAtPhysicalStartOfLine) {
LexNextToken:
  // New token, can't need cleaning yet.
  Result.clearFlag(Token::NeedsCleaning);
  Result.setIdentifierInfo(nullptr);

  // CurPtr - Cache BufferPtr in an automatic variable.
  unsigned CurOffset = BufferOffset;

  // Small amounts of horizontal whitespace is very common between tokens.
  if (isHorizontalWhitespace(BufferStart[CurOffset])) {
    do {
      do {
        ++CurOffset;
      } while (isHorizontalWhitespace(BufferStart[CurOffset]));
    } while (BufferStart[CurOffset] == 0 && CurOffset == BufferSize && TryExpandBuffer());

    // If we are keeping whitespace and other tokens, just return what we just
    // skipped.  The next lexer invocation will return the token after the
    // whitespace.
    if (isKeepWhitespaceMode()) {
      FormTokenWithChars(Result, CurOffset, tok::unknown);
      // FIXME: The next token will not have LeadingSpace set.
      return true;
    }

    BufferOffset = CurOffset;
    Result.setFlag(Token::LeadingSpace);
  }

  unsigned SizeTmp, SizeTmp2;   // Temporaries for use in cases below.

  // Read a character, advancing over it.
  char Char = getAndAdvanceChar(CurOffset, Result);
  tok::TokenKind Kind;

  if (!isVerticalWhitespace(Char))
    NewLineOffset = llvm::None;

  switch (Char) {
  case 0:  // Null.
    // Found end of file?
    if (CurOffset-1 == BufferSize) {
      if (TryExpandBuffer()) {
        goto LexNextToken;
      }
      return LexEndOfFile(Result, CurOffset-1);
    }

    // Check if we are performing code completion.
    if (isCodeCompletionPoint(CurOffset-1)) {
      // Return the code-completion token.
      Result.startToken();
      FormTokenWithChars(Result, CurOffset, tok::code_completion);
      return true;
    }

    if (!isLexingRawMode())
      Diag(CurOffset-1, diag::null_in_file);
    Result.setFlag(Token::LeadingSpace);
    if (SkipWhitespace(Result, CurOffset, TokAtPhysicalStartOfLine))
      return true; // KeepWhitespaceMode

    // We know the lexer hasn't changed, so just try again with this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;

  case 26:  // DOS & CP/M EOF: "^Z".
    // If we're in Microsoft extensions mode, treat this as end of file.
    if (LangOpts.MicrosoftExt) {
      if (!isLexingRawMode())
        Diag(CurOffset-1, diag::ext_ctrl_z_eof_microsoft);
      return LexEndOfFile(Result, CurOffset-1);
    }

    // If Microsoft extensions are disabled, this is just random garbage.
    Kind = tok::unknown;
    break;

  case '\r':
    if (BufferStart[CurOffset] == '\n')
      (void)getAndAdvanceChar(CurOffset, Result);
    [[fallthrough]];
  case '\n':
    // If we are inside a preprocessor directive and we see the end of line,
    // we know we are done with the directive, so return an EOD token.
    if (ParsingPreprocessorDirective) {
      // Done parsing the "line".
      ParsingPreprocessorDirective = false;

      // Restore comment saving mode, in case it was disabled for directive.
      if (PP)
        resetExtendedTokenMode();

      // Since we consumed a newline, we are back at the start of a line.
      IsAtStartOfLine = true;
      IsAtPhysicalStartOfLine = true;
      NewLineOffset = CurOffset - 1;

      Kind = tok::eod;
      break;
    }

    // No leading whitespace seen so far.
    Result.clearFlag(Token::LeadingSpace);

    if (SkipWhitespace(Result, CurOffset, TokAtPhysicalStartOfLine))
      return true; // KeepWhitespaceMode

    // We only saw whitespace, so just try again with this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;
  case ' ':
  case '\t':
  case '\f':
  case '\v':
  SkipHorizontalWhitespace:
    Result.setFlag(Token::LeadingSpace);
    if (SkipWhitespace(Result, CurOffset, TokAtPhysicalStartOfLine))
      return true; // KeepWhitespaceMode

  SkipIgnoredUnits:
    CurOffset = BufferOffset;

    // If the next token is obviously a // or /* */ comment, skip it efficiently
    // too (without going through the big switch stmt).
    if (BufferStart[CurOffset] == '/' && BufferStart[CurOffset+1] == '/' && !inKeepCommentMode() &&
        LineComment && (LangOpts.CPlusPlus || !LangOpts.TraditionalCPP)) {
      if (SkipLineComment(Result, CurOffset+2, TokAtPhysicalStartOfLine))
        return true; // There is a token to return.
      goto SkipIgnoredUnits;
    } else if (BufferStart[CurOffset] == '/' && BufferStart[CurOffset+1] == '*' && !inKeepCommentMode()) {
      if (SkipBlockComment(Result, CurOffset+2, TokAtPhysicalStartOfLine))
        return true; // There is a token to return.
      goto SkipIgnoredUnits;
    } else if (isHorizontalWhitespace(BufferStart[CurOffset])) {
      goto SkipHorizontalWhitespace;
    }
    // We only saw whitespace, so just try again with this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;

  // C99 6.4.4.1: Integer Constants.
  // C99 6.4.4.2: Floating Constants.
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();
    return LexNumericConstant(Result, CurOffset);

  // Identifier (e.g., uber), or
  // UTF-8 (C2x/C++17) or UTF-16 (C11/C++11) character literal, or
  // UTF-8 or UTF-16 string literal (C11/C++11).
  case 'u':
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();

    if (LangOpts.CPlusPlus11 || LangOpts.C11) {
      Char = getCharAndSize(CurOffset, SizeTmp);

      // UTF-16 string literal
      if (Char == '"')
        return LexStringLiteral(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                                tok::utf16_string_literal);

      // UTF-16 character constant
      if (Char == '\'')
        return LexCharConstant(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                               tok::utf16_char_constant);

      // UTF-16 raw string literal
      if (Char == 'R' && LangOpts.CPlusPlus11 &&
          getCharAndSize(CurOffset + SizeTmp, SizeTmp2) == '"')
        return LexRawStringLiteral(Result,
                               ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                                           SizeTmp2, Result),
                               tok::utf16_string_literal);

      if (Char == '8') {
        char Char2 = getCharAndSize(CurOffset + SizeTmp, SizeTmp2);

        // UTF-8 string literal
        if (Char2 == '"')
          return LexStringLiteral(Result,
                               ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                                           SizeTmp2, Result),
                               tok::utf8_string_literal);
        if (Char2 == '\'' && (LangOpts.CPlusPlus17 || LangOpts.C2x))
          return LexCharConstant(
              Result, ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                                  SizeTmp2, Result),
              tok::utf8_char_constant);

        if (Char2 == 'R' && LangOpts.CPlusPlus11) {
          unsigned SizeTmp3;
          char Char3 = getCharAndSize(CurOffset + SizeTmp + SizeTmp2, SizeTmp3);
          // UTF-8 raw string literal
          if (Char3 == '"') {
            return LexRawStringLiteral(Result,
                   ConsumeChar(ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                                           SizeTmp2, Result),
                               SizeTmp3, Result),
                   tok::utf8_string_literal);
          }
        }
      }
    }

    // treat u like the start of an identifier.
    return LexIdentifierContinue(Result, CurOffset);

  case 'U': // Identifier (e.g. Uber) or C11/C++11 UTF-32 string literal
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();

    if (LangOpts.CPlusPlus11 || LangOpts.C11) {
      Char = getCharAndSize(CurOffset, SizeTmp);

      // UTF-32 string literal
      if (Char == '"')
        return LexStringLiteral(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                                tok::utf32_string_literal);

      // UTF-32 character constant
      if (Char == '\'')
        return LexCharConstant(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                               tok::utf32_char_constant);

      // UTF-32 raw string literal
      if (Char == 'R' && LangOpts.CPlusPlus11 &&
          getCharAndSize(CurOffset + SizeTmp, SizeTmp2) == '"')
        return LexRawStringLiteral(Result,
                               ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                                           SizeTmp2, Result),
                               tok::utf32_string_literal);
    }

    // treat U like the start of an identifier.
    return LexIdentifierContinue(Result, CurOffset);

  case 'R': // Identifier or C++0x raw string literal
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();

    if (LangOpts.CPlusPlus11) {
      Char = getCharAndSize(CurOffset, SizeTmp);

      if (Char == '"')
        return LexRawStringLiteral(Result,
                                   ConsumeChar(CurOffset, SizeTmp, Result),
                                   tok::string_literal);
    }

    // treat R like the start of an identifier.
    return LexIdentifierContinue(Result, CurOffset);

  case 'L':   // Identifier (Loony) or wide literal (L'x' or L"xyz").
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();
    Char = getCharAndSize(CurOffset, SizeTmp);

    // Wide string literal.
    if (Char == '"')
      return LexStringLiteral(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                              tok::wide_string_literal);

    // Wide raw string literal.
    if (LangOpts.CPlusPlus11 && Char == 'R' &&
        getCharAndSize(CurOffset + SizeTmp, SizeTmp2) == '"')
      return LexRawStringLiteral(Result,
                               ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                                           SizeTmp2, Result),
                               tok::wide_string_literal);

    // Wide character constant.
    if (Char == '\'')
      return LexCharConstant(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                             tok::wide_char_constant);
    // FALL THROUGH, treating L like the start of an identifier.
    [[fallthrough]];

  // C99 6.4.2: Identifiers.
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
  case 'H': case 'I': case 'J': case 'K':    /*'L'*/case 'M': case 'N':
  case 'O': case 'P': case 'Q':    /*'R'*/case 'S': case 'T':    /*'U'*/
  case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
  case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
  case 'o': case 'p': case 'q': case 'r': case 's': case 't':    /*'u'*/
  case 'v': case 'w': case 'x': case 'y': case 'z':
  case '_':
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();
    return LexIdentifierContinue(Result, CurOffset);

  case '$':   // $ in identifiers.
    if (LangOpts.DollarIdents) {
      if (!isLexingRawMode())
        Diag(CurOffset-1, diag::ext_dollar_in_identifier);
      // Notify MIOpt that we read a non-whitespace/non-comment token.
      MIOpt.ReadToken();
      return LexIdentifierContinue(Result, CurOffset);
    }

    Kind = tok::unknown;
    break;

  // C99 6.4.4: Character Constants.
  case '\'':
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();
    return LexCharConstant(Result, CurOffset, tok::char_constant);

  // C99 6.4.5: String Literals.
  case '"':
    // Notify MIOpt that we read a non-whitespace/non-comment token.
    MIOpt.ReadToken();
    return LexStringLiteral(Result, CurOffset,
                            ParsingFilename ? tok::header_name
                                            : tok::string_literal);

  // C99 6.4.6: Punctuators.
  case '?':
    Kind = tok::question;
    break;
  case '[':
    Kind = tok::l_square;
    break;
  case ']':
    Kind = tok::r_square;
    break;
  case '(':
    Kind = tok::l_paren;
    break;
  case ')':
    Kind = tok::r_paren;
    break;
  case '{':
    Kind = tok::l_brace;
    break;
  case '}':
    Kind = tok::r_brace;
    break;
  case '.':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char >= '0' && Char <= '9') {
      // Notify MIOpt that we read a non-whitespace/non-comment token.
      MIOpt.ReadToken();

      return LexNumericConstant(Result, ConsumeChar(CurOffset, SizeTmp, Result));
    } else if (LangOpts.CPlusPlus && Char == '*') {
      Kind = tok::periodstar;
      CurOffset += SizeTmp;
    } else if (Char == '.' &&
               getCharAndSize(CurOffset+SizeTmp, SizeTmp2) == '.') {
      Kind = tok::ellipsis;
      CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                           SizeTmp2, Result);
    } else {
      Kind = tok::period;
    }
    break;
  case '&':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '&') {
      Kind = tok::ampamp;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else if (Char == '=') {
      Kind = tok::ampequal;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else {
      Kind = tok::amp;
    }
    break;
  case '*':
    if (getCharAndSize(CurOffset, SizeTmp) == '=') {
      Kind = tok::starequal;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else {
      Kind = tok::star;
    }
    break;
  case '+':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '+') {
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::plusplus;
    } else if (Char == '=') {
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::plusequal;
    } else {
      Kind = tok::plus;
    }
    break;
  case '-':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '-') {      // --
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::minusminus;
    } else if (Char == '>' && LangOpts.CPlusPlus &&
               getCharAndSize(CurOffset+SizeTmp, SizeTmp2) == '*') {  // C++ ->*
      CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                           SizeTmp2, Result);
      Kind = tok::arrowstar;
    } else if (Char == '>') {   // ->
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::arrow;
    } else if (Char == '=') {   // -=
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::minusequal;
    } else {
      Kind = tok::minus;
    }
    break;
  case '~':
    Kind = tok::tilde;
    break;
  case '!':
    if (getCharAndSize(CurOffset, SizeTmp) == '=') {
      Kind = tok::exclaimequal;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else {
      Kind = tok::exclaim;
    }
    break;
  case '/':
    // 6.4.9: Comments
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '/') {         // Line comment.
      // Even if Line comments are disabled (e.g. in C89 mode), we generally
      // want to lex this as a comment.  There is one problem with this though,
      // that in one particular corner case, this can change the behavior of the
      // resultant program.  For example, In  "foo //**/ bar", C89 would lex
      // this as "foo / bar" and languages with Line comments would lex it as
      // "foo".  Check to see if the character after the second slash is a '*'.
      // If so, we will lex that as a "/" instead of the start of a comment.
      // However, we never do this if we are just preprocessing.
      bool TreatAsComment =
          LineComment && (LangOpts.CPlusPlus || !LangOpts.TraditionalCPP);
      if (!TreatAsComment)
        if (!(PP && PP->isPreprocessedOutput()))
          TreatAsComment = getCharAndSize(CurOffset+SizeTmp, SizeTmp2) != '*';

      if (TreatAsComment) {
        if (SkipLineComment(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                            TokAtPhysicalStartOfLine))
          return true; // There is a token to return.

        // It is common for the tokens immediately after a // comment to be
        // whitespace (indentation for the next line).  Instead of going through
        // the big switch, handle it efficiently now.
        goto SkipIgnoredUnits;
      }
    }

    if (Char == '*') {  // /**/ comment.
      if (SkipBlockComment(Result, ConsumeChar(CurOffset, SizeTmp, Result),
                           TokAtPhysicalStartOfLine))
        return true; // There is a token to return.

      // We only saw whitespace, so just try again with this lexer.
      // (We manually eliminate the tail call to avoid recursion.)
      goto LexNextToken;
    }

    if (Char == '=') {
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::slashequal;
    } else {
      Kind = tok::slash;
    }
    break;
  case '%':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '=') {
      Kind = tok::percentequal;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else if (LangOpts.Digraphs && Char == '>') {
      Kind = tok::r_brace;                             // '%>' -> '}'
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else if (LangOpts.Digraphs && Char == ':') {
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Char = getCharAndSize(CurOffset, SizeTmp);
      if (Char == '%' && getCharAndSize(CurOffset+SizeTmp, SizeTmp2) == ':') {
        Kind = tok::hashhash;                          // '%:%:' -> '##'
        CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                             SizeTmp2, Result);
      } else if (Char == '@' && LangOpts.MicrosoftExt) {// %:@ -> #@ -> Charize
        CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
        if (!isLexingRawMode())
          Diag(BufferOffset, diag::ext_charize_microsoft);
        Kind = tok::hashat;
      } else {                                         // '%:' -> '#'
        // We parsed a # character.  If this occurs at the start of the line,
        // it's actually the start of a preprocessing directive.  Callback to
        // the preprocessor to handle it.
        // TODO: -fpreprocessed mode??
        if (TokAtPhysicalStartOfLine && !LexingRawMode && !Is_PragmaLexer)
          goto HandleDirective;

        Kind = tok::hash;
      }
    } else {
      Kind = tok::percent;
    }
    break;
  case '<':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (ParsingFilename) {
      return LexAngledStringLiteral(Result, CurOffset);
    } else if (Char == '<') {
      char After = getCharAndSize(CurOffset+SizeTmp, SizeTmp2);
      if (After == '=') {
        Kind = tok::lesslessequal;
        CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                             SizeTmp2, Result);
      } else if (After == '<' && IsStartOfConflictMarker(CurOffset-1)) {
        // If this is actually a '<<<<<<<' version control conflict marker,
        // recognize it as such and recover nicely.
        goto LexNextToken;
      } else if (After == '<' && HandleEndOfConflictMarker(CurOffset-1)) {
        // If this is '<<<<' and we're in a Perforce-style conflict marker,
        // ignore it.
        goto LexNextToken;
      } else if (LangOpts.CUDA && After == '<') {
        Kind = tok::lesslessless;
        CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                             SizeTmp2, Result);
      } else {
        CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
        Kind = tok::lessless;
      }
    } else if (Char == '=') {
      char After = getCharAndSize(CurOffset+SizeTmp, SizeTmp2);
      if (After == '>') {
        if (LangOpts.CPlusPlus20) {
          if (!isLexingRawMode())
            Diag(BufferOffset, diag::warn_cxx17_compat_spaceship);
          CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                               SizeTmp2, Result);
          Kind = tok::spaceship;
          break;
        }
        // Suggest adding a space between the '<=' and the '>' to avoid a
        // change in semantics if this turns up in C++ <=17 mode.
        if (LangOpts.CPlusPlus && !isLexingRawMode()) {
          Diag(BufferOffset, diag::warn_cxx20_compat_spaceship)
            << FixItHint::CreateInsertion(
                   getSourceLocation(CurOffset + SizeTmp, SizeTmp2), " ");
        }
      }
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::lessequal;
    } else if (LangOpts.Digraphs && Char == ':') {     // '<:' -> '['
      if (LangOpts.CPlusPlus11 &&
          getCharAndSize(CurOffset + SizeTmp, SizeTmp2) == ':') {
        // C++0x [lex.pptoken]p3:
        //  Otherwise, if the next three characters are <:: and the subsequent
        //  character is neither : nor >, the < is treated as a preprocessor
        //  token by itself and not as the first character of the alternative
        //  token <:.
        unsigned SizeTmp3;
        char After = getCharAndSize(CurOffset + SizeTmp + SizeTmp2, SizeTmp3);
        if (After != ':' && After != '>') {
          Kind = tok::less;
          if (!isLexingRawMode())
            Diag(BufferOffset, diag::warn_cxx98_compat_less_colon_colon);
          break;
        }
      }

      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::l_square;
    } else if (LangOpts.Digraphs && Char == '%') {     // '<%' -> '{'
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::l_brace;
    } else if (Char == '#' && /*Not a trigraph*/ SizeTmp == 1 &&
               lexEditorPlaceholder(Result, CurOffset)) {
      return true;
    } else {
      Kind = tok::less;
    }
    break;
  case '>':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '=') {
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::greaterequal;
    } else if (Char == '>') {
      char After = getCharAndSize(CurOffset+SizeTmp, SizeTmp2);
      if (After == '=') {
        CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                             SizeTmp2, Result);
        Kind = tok::greatergreaterequal;
      } else if (After == '>' && IsStartOfConflictMarker(CurOffset-1)) {
        // If this is actually a '>>>>' conflict marker, recognize it as such
        // and recover nicely.
        goto LexNextToken;
      } else if (After == '>' && HandleEndOfConflictMarker(CurOffset-1)) {
        // If this is '>>>>>>>' and we're in a conflict marker, ignore it.
        goto LexNextToken;
      } else if (LangOpts.CUDA && After == '>') {
        Kind = tok::greatergreatergreater;
        CurOffset = ConsumeChar(ConsumeChar(CurOffset, SizeTmp, Result),
                             SizeTmp2, Result);
      } else {
        CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
        Kind = tok::greatergreater;
      }
    } else {
      Kind = tok::greater;
    }
    break;
  case '^':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '=') {
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::caretequal;
    } else if (LangOpts.OpenCL && Char == '^') {
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
      Kind = tok::caretcaret;
    } else {
      Kind = tok::caret;
    }
    break;
  case '|':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '=') {
      Kind = tok::pipeequal;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else if (Char == '|') {
      // If this is '|||||||' and we're in a conflict marker, ignore it.
      if (BufferStart[CurOffset+1] == '|' && HandleEndOfConflictMarker(CurOffset-1))
        goto LexNextToken;
      Kind = tok::pipepipe;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else {
      Kind = tok::pipe;
    }
    break;
  case ':':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (LangOpts.Digraphs && Char == '>') {
      Kind = tok::r_square; // ':>' -> ']'
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else if ((LangOpts.CPlusPlus ||
                LangOpts.DoubleSquareBracketAttributes) &&
               Char == ':') {
      Kind = tok::coloncolon;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else {
      Kind = tok::colon;
    }
    break;
  case ';':
    Kind = tok::semi;
    break;
  case '=':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '=') {
      // If this is '====' and we're in a conflict marker, ignore it.
      if (BufferStart[CurOffset+1] == '=' && HandleEndOfConflictMarker(CurOffset-1))
        goto LexNextToken;

      Kind = tok::equalequal;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else {
      Kind = tok::equal;
    }
    break;
  case ',':
    Kind = tok::comma;
    break;
  case '#':
    Char = getCharAndSize(CurOffset, SizeTmp);
    if (Char == '#') {
      Kind = tok::hashhash;
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else if (Char == '@' && LangOpts.MicrosoftExt) {  // #@ -> Charize
      Kind = tok::hashat;
      if (!isLexingRawMode())
        Diag(BufferOffset, diag::ext_charize_microsoft);
      CurOffset = ConsumeChar(CurOffset, SizeTmp, Result);
    } else {
      // We parsed a # character.  If this occurs at the start of the line,
      // it's actually the start of a preprocessing directive.  Callback to
      // the preprocessor to handle it.
      // TODO: -fpreprocessed mode??
      if (TokAtPhysicalStartOfLine && !LexingRawMode && !Is_PragmaLexer)
        goto HandleDirective;

      Kind = tok::hash;
    }
    break;

  case '@':
    // Objective C support.
    if (BufferStart[CurOffset-1] == '@' && LangOpts.ObjC)
      Kind = tok::at;
    else
      Kind = tok::unknown;
    break;

  // UCNs (C99 6.4.3, C++11 [lex.charset]p2)
  case '\\':
    if (!LangOpts.AsmPreprocessor) {
      if (uint32_t CodePoint = tryReadUCN(CurOffset, BufferOffset, &Result)) {
        if (CheckUnicodeWhitespace(Result, CodePoint, CurOffset)) {
          if (SkipWhitespace(Result, CurOffset, TokAtPhysicalStartOfLine))
            return true; // KeepWhitespaceMode

          // We only saw whitespace, so just try again with this lexer.
          // (We manually eliminate the tail call to avoid recursion.)
          goto LexNextToken;
        }

        return LexUnicodeIdentifierStart(Result, CodePoint, CurOffset);
      }
    }

    Kind = tok::unknown;
    break;

  default: {
    if (isASCII(Char)) {
      Kind = tok::unknown;
      break;
    }

    llvm::UTF32 CodePoint;

    // We can't just reset CurPtr to BufferPtr because BufferPtr may point to
    // an escaped newline.
    --CurOffset;
    const char* CurPtr = BufferStart + CurOffset;
    llvm::ConversionResult Status =
        llvm::convertUTF8Sequence((const llvm::UTF8 **)&CurPtr,
                                  (const llvm::UTF8 *)(BufferStart + BufferSize),
                                  &CodePoint,
                                  llvm::strictConversion);
    CurOffset = CurPtr-BufferStart;
    if (Status == llvm::conversionOK) {
      if (CheckUnicodeWhitespace(Result, CodePoint, CurOffset)) {
        if (SkipWhitespace(Result, CurOffset, TokAtPhysicalStartOfLine))
          return true; // KeepWhitespaceMode

        // We only saw whitespace, so just try again with this lexer.
        // (We manually eliminate the tail call to avoid recursion.)
        goto LexNextToken;
      }
      return LexUnicodeIdentifierStart(Result, CodePoint, CurOffset);
    }

    if (isLexingRawMode() || ParsingPreprocessorDirective ||
        PP->isPreprocessedOutput()) {
      ++CurOffset;
      Kind = tok::unknown;
      break;
    }

    // Non-ASCII characters tend to creep into source code unintentionally.
    // Instead of letting the parser complain about the unknown token,
    // just diagnose the invalid UTF-8, then drop the character.
    Diag(CurOffset, diag::err_invalid_utf8);

    BufferOffset = CurOffset+1;
    // We're pretending the character didn't exist, so just try again with
    // this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;
  }
  }

  // Notify MIOpt that we read a non-whitespace/non-comment token.
  MIOpt.ReadToken();

  // Update the location of token as well as BufferPtr.
  FormTokenWithChars(Result, CurOffset, Kind);
  return true;

HandleDirective:
  // We parsed a # character and it's the start of a preprocessing directive.

  FormTokenWithChars(Result, CurOffset, tok::hash);
  PP->HandleDirective(Result);

  if (PP->hadModuleLoaderFatalFailure()) {
    // With a fatal failure in the module loader, we abort parsing.
    assert(Result.is(tok::eof) && "Preprocessor did not set tok:eof");
    return true;
  }

  // We parsed the directive; lex a token with the new state.
  return false;
}

const char *Lexer::convertDependencyDirectiveToken(
    const dependency_directives_scan::Token &DDTok, Token &Result) {
  const char *TokPtr = BufferStart + DDTok.Offset;
  Result.startToken();
  Result.setLocation(getSourceLocation(TokPtr-BufferStart));
  Result.setKind(DDTok.Kind);
  Result.setFlag((Token::TokenFlags)DDTok.Flags);
  Result.setLength(DDTok.Length);
  BufferOffset = TokPtr + DDTok.Length - BufferStart;
  return TokPtr;
}

bool Lexer::LexDependencyDirectiveToken(Token &Result) {
  assert(isDependencyDirectivesLexer());

  using namespace dependency_directives_scan;

  while (NextDepDirectiveTokenIndex == DepDirectives.front().Tokens.size()) {
    if (DepDirectives.front().Kind == pp_eof)
      return LexEndOfFile(Result, BufferSize);
    if (DepDirectives.front().Kind == tokens_present_before_eof)
      MIOpt.ReadToken();
    NextDepDirectiveTokenIndex = 0;
    DepDirectives = DepDirectives.drop_front();
  }

  const dependency_directives_scan::Token &DDTok =
      DepDirectives.front().Tokens[NextDepDirectiveTokenIndex++];
  if (NextDepDirectiveTokenIndex > 1 || DDTok.Kind != tok::hash) {
    // Read something other than a preprocessor directive hash.
    MIOpt.ReadToken();
  }

  const char *TokPtr = convertDependencyDirectiveToken(DDTok, Result);

  if (Result.is(tok::hash) && Result.isAtStartOfLine()) {
    PP->HandleDirective(Result);
    return false;
  }
  if (Result.is(tok::raw_identifier)) {
    Result.setRawIdentifierData(TokPtr);
    if (!isLexingRawMode()) {
      IdentifierInfo *II = PP->LookUpIdentifierInfo(Result);
      if (II->isHandleIdentifierCase())
        return PP->HandleIdentifier(Result);
    }
    return true;
  }
  if (Result.isLiteral()) {
    SetTokString(Result, StringRef(TokPtr, Result.getLength()));
    return true;
  }
  if (Result.is(tok::colon) &&
      (LangOpts.CPlusPlus || LangOpts.DoubleSquareBracketAttributes)) {
    // Convert consecutive colons to 'tok::coloncolon'.
    if (BufferStart[BufferOffset] == ':') {
      assert(DepDirectives.front().Tokens[NextDepDirectiveTokenIndex].is(
          tok::colon));
      ++NextDepDirectiveTokenIndex;
      Result.setKind(tok::coloncolon);
    }
    return true;
  }
  if (Result.is(tok::eod))
    ParsingPreprocessorDirective = false;

  return true;
}

bool Lexer::LexDependencyDirectiveTokenWhileSkipping(Token &Result) {
  assert(isDependencyDirectivesLexer());

  using namespace dependency_directives_scan;

  bool Stop = false;
  unsigned NestedIfs = 0;
  do {
    DepDirectives = DepDirectives.drop_front();
    switch (DepDirectives.front().Kind) {
    case pp_none:
      llvm_unreachable("unexpected 'pp_none'");
    case pp_include:
    case pp___include_macros:
    case pp_define:
    case pp_undef:
    case pp_import:
    case pp_pragma_import:
    case pp_pragma_once:
    case pp_pragma_push_macro:
    case pp_pragma_pop_macro:
    case pp_pragma_include_alias:
    case pp_include_next:
    case decl_at_import:
    case cxx_module_decl:
    case cxx_import_decl:
    case cxx_export_module_decl:
    case cxx_export_import_decl:
    case tokens_present_before_eof:
      break;
    case pp_if:
    case pp_ifdef:
    case pp_ifndef:
      ++NestedIfs;
      break;
    case pp_elif:
    case pp_elifdef:
    case pp_elifndef:
    case pp_else:
      if (!NestedIfs) {
        Stop = true;
      }
      break;
    case pp_endif:
      if (!NestedIfs) {
        Stop = true;
      } else {
        --NestedIfs;
      }
      break;
    case pp_eof:
      NextDepDirectiveTokenIndex = 0;
      return LexEndOfFile(Result, BufferSize);
    }
  } while (!Stop);

  const dependency_directives_scan::Token &DDTok =
      DepDirectives.front().Tokens.front();
  assert(DDTok.is(tok::hash));
  NextDepDirectiveTokenIndex = 1;

  convertDependencyDirectiveToken(DDTok, Result);
  return false;
}
