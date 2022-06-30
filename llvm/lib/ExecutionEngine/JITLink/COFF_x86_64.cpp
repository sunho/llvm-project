//===----- COFF_x86_64.cpp - JIT linker implementation for ELF/aarch64 ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// COFF/x86_64 jit-link implementation.
//
//===----------------------------------------------------------------------===//

#include "llvm/ExecutionEngine/JITLink/COFF_x86_64.h"
#include "COFFLinkGraphBuilder.h"
#include "EHFrameSupportImpl.h"
#include "JITLinkGeneric.h"
#include "llvm/BinaryFormat/COFF.h"
#include "llvm/ExecutionEngine/JITLink/x86_64.h"
#include "llvm/Object/COFF.h"
#include "llvm/Support/Endian.h"

#define DEBUG_TYPE "jitlink"

using namespace llvm;
using namespace llvm::jitlink;

namespace {

class COFFJITLinker_x86_64 : public JITLinker<COFFJITLinker_x86_64> {
  friend class JITLinker<COFFJITLinker_x86_64>;

public:
  COFFJITLinker_x86_64(std::unique_ptr<JITLinkContext> Ctx,
                       std::unique_ptr<LinkGraph> G,
                       PassConfiguration PassConfig)
      : JITLinker(std::move(Ctx), std::move(G), std::move(PassConfig)) {}

private:
  Error applyFixup(LinkGraph &G, Block &B, const Edge &E) const {
    return x86_64::applyFixup(G, B, E, nullptr);
  }
};

class COFFLinkGraphBuilder_x86_64 : public COFFLinkGraphBuilder {
private:
  enum COFFX86RelocationKind : Edge::Kind {
    COFFAddr32NB = Edge::FirstRelocation,
  };

  static Expected<COFFX86RelocationKind>
  getRelocationKind(const uint32_t Type) {
    switch (Type) {
    case COFF::RelocationTypeAMD64::IMAGE_REL_AMD64_ADDR32NB:
      return COFFAddr32NB;
    }

    return make_error<JITLinkError>("Unsupported x86_64 relocation:" +
                                    formatv("{0:d}", Type));
  }

  Error addRelocations() override {

    LLVM_DEBUG(dbgs() << "Processing relocations:\n");

    for (const auto &RelSect : sections())
      if (Error Err = COFFLinkGraphBuilder::forEachRelocation(
              RelSect, this, &COFFLinkGraphBuilder_x86_64::addSingleRelocation))
        return Err;

    return Error::success();
  }

  Error addSingleRelocation(const object::RelocationRef &Rel,
                            const object::SectionRef &FixupSect,
                            Block &BlockToFix) {

    const object::coff_relocation *COFFRel = getObject().getCOFFRelocation(Rel);
    const auto SymbolIt = Rel.getSymbol();
    if (SymbolIt == getObject().symbol_end()) {
      return make_error<StringError>(
          formatv("Invalid symbol index in relocation entry. "
                  "index: {0}, section: {1}",
                  COFFRel->SymbolTableIndex, FixupSect.getIndex()),
          inconvertibleErrorCode());
    }

    const object::COFFSymbolRef COFFSymbol =
        getObject().getCOFFSymbol(*SymbolIt);
    const COFFSymbolIndex SymIndex = getObject().getSymbolIndex(COFFSymbol);

    Symbol *GraphSymbol = getGraphSymbol(SymIndex);
    if (!GraphSymbol)
      return make_error<StringError>(
          formatv("Could not find symbol at given index, did you add it to "
                  "JITSymbolTable? index: {0}, section: {1}",
                  SymIndex, FixupSect.getIndex()),
          inconvertibleErrorCode());

    Expected<COFFX86RelocationKind> RelocKind =
        getRelocationKind(Rel.getType());
    if (!RelocKind)
      return RelocKind.takeError();

    int64_t Addend = 0;
    orc::ExecutorAddr FixupAddress =
        orc::ExecutorAddr(FixupSect.getAddress()) + Rel.getOffset();
    Edge::OffsetT Offset = FixupAddress - BlockToFix.getAddress();

    // Get a pointer to the fixup content.
    const void *FixupContent = BlockToFix.getContent().data() +
                               (FixupAddress - BlockToFix.getAddress());

    Edge::Kind Kind = Edge::Invalid;

    switch (*RelocKind) {

    case COFFAddr32NB: {
      Kind = x86_64::Pointer32;
      break;
    }
    };

    Edge GE(Kind, Offset, *GraphSymbol, Addend);
    LLVM_DEBUG({
      dbgs() << "    ";
      printEdge(dbgs(), BlockToFix, GE, x86_64::getEdgeKindName(Kind));
      dbgs() << "\n";
    });

    BlockToFix.addEdge(std::move(GE));
    return Error::success();
  }

  /// Return the string name of the given ELF aarch64 edge kind.
  const char *getCOFFX86RelocationKindName(Edge::Kind R) {
    switch (R) {
    case COFFAddr32NB:
      return "COFFAddr32NB";
    default:
      return getGenericEdgeKindName(static_cast<Edge::Kind>(R));
    }
  }

public:
  COFFLinkGraphBuilder_x86_64(const object::COFFObjectFile &Obj, const Triple T)
      : COFFLinkGraphBuilder(Obj, std::move(T), x86_64::getEdgeKindName) {}
};

} // namespace

namespace llvm {
namespace jitlink {

Expected<std::unique_ptr<LinkGraph>>
createLinkGraphFromCOFFObject_x86_64(MemoryBufferRef ObjectBuffer) {
  LLVM_DEBUG({
    dbgs() << "Building jitlink graph for new input "
           << ObjectBuffer.getBufferIdentifier() << "...\n";
  });

  auto COFFObj = object::ObjectFile::createCOFFObjectFile(ObjectBuffer);
  if (!COFFObj)
    return COFFObj.takeError();

  return COFFLinkGraphBuilder_x86_64(**COFFObj, (*COFFObj)->makeTriple())
      .buildGraph();
}

void link_COFF_x86_64(std::unique_ptr<LinkGraph> G,
                      std::unique_ptr<JITLinkContext> Ctx) {
  PassConfiguration Config;
  const Triple &TT = G->getTargetTriple();
  if (Ctx->shouldAddDefaultTargetPasses(TT)) {
    // Add a mark-live pass.
    if (auto MarkLive = Ctx->getMarkLivePass(TT))
      Config.PrePrunePasses.push_back(std::move(MarkLive));
    else
      Config.PrePrunePasses.push_back(markAllSymbolsLive);
  }

  if (auto Err = Ctx->modifyPassConfig(*G, Config))
    return Ctx->notifyFailed(std::move(Err));

  COFFJITLinker_x86_64::link(std::move(Ctx), std::move(G), std::move(Config));
}

} // namespace jitlink
} // namespace llvm
