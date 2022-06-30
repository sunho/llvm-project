//===-------------- COFF.cpp - JIT linker function for COFF -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// COFF jit-link function.
//
//===----------------------------------------------------------------------===//

#include "llvm/ExecutionEngine/JITLink/COFF.h"

#include "llvm/BinaryFormat/COFF.h"
#include "llvm/ExecutionEngine/JITLink/COFF_x86_64.h"
#include "llvm/Object/COFF.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/MemoryBuffer.h"
#include <cstring>

using namespace llvm;

#define DEBUG_TYPE "jitlink"

namespace llvm {
namespace jitlink {

Expected<std::unique_ptr<LinkGraph>>
createLinkGraphFromCOFFObject(MemoryBufferRef ObjectBuffer) {
  StringRef Buffer = ObjectBuffer.getBuffer();
  /*
  std::error_code EC;
  if (Buffer.size() < sizeof(object::coff_file_header))
     return make_error<JITLinkError>("Truncated COFF buffer");

  Expected<uint16_t> TargetMachineArch = readTargetMachineArch(Buffer);
  if (!TargetMachineArch)
      return TargetMachineArch.takeError();*/
  /*
  switch (*TargetMachineArch) {
  case ELF::EM_X86_64:
      return createLinkGraphFromCOFFObject_x86_64(ObjectBuffer);
  default:
      return make_error<JITLinkError>(
          "Unsupported target machine architecture in COFF object " +
          ObjectBuffer.getBufferIdentifier());
  }*/
  return createLinkGraphFromCOFFObject_x86_64(ObjectBuffer);
}

void link_COFF(std::unique_ptr<LinkGraph> G,
               std::unique_ptr<JITLinkContext> Ctx) {
  switch (G->getTargetTriple().getArch()) {
  case Triple::x86_64:
    link_COFF_x86_64(std::move(G), std::move(Ctx));
    return;
  default:
    Ctx->notifyFailed(make_error<JITLinkError>(
        "Unsupported target machine architecture in COFF link graph " +
        G->getName()));
    return;
  }
}

} // end namespace jitlink
} // end namespace llvm
