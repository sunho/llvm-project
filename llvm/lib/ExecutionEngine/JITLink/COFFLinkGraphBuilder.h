//===----- COFFLinkGraphBuilder.h - COFF LinkGraph builder ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Generic COFF LinkGraph building code.
//
//===----------------------------------------------------------------------===//

#ifndef LIB_EXECUTIONENGINE_JITLINK_COFFLINKGRAPHBUILDER_H
#define LIB_EXECUTIONENGINE_JITLINK_COFFLINKGRAPHBUILDER_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ExecutionEngine/JITLink/JITLink.h"
#include "llvm/Object/COFF.h"

#include "EHFrameSupportImpl.h"
#include "JITLinkGeneric.h"

#define DEBUG_TYPE "jitlink"

#include <list>

namespace llvm {
namespace jitlink {

class COFFLinkGraphBuilder {
public:
  virtual ~COFFLinkGraphBuilder();
  Expected<std::unique_ptr<LinkGraph>> buildGraph();

protected:
  using COFFSectionIndex = int32_t;
  using COFFSymbolIndex = int32_t;

  COFFLinkGraphBuilder(const object::COFFObjectFile &Obj, Triple TT,
                       LinkGraph::GetEdgeKindNameFunction GetEdgeKindName);

  LinkGraph &getGraph() const { return *G; }

  const object::COFFObjectFile &getObject() const { return Obj; }

  virtual Error addRelocations() = 0;

  Error graphifySections();
  Error graphifySymbols();

  void setGraphSymbol(COFFSymbolIndex SymIndex, Symbol &Sym) {
    assert(!GraphSymbols[SymIndex] && "Duplicate symbol at index");
    GraphSymbols[SymIndex] = &Sym;
  }

  Symbol *getGraphSymbol(COFFSymbolIndex SymIndex) const {
    if (SymIndex < 0 || SymIndex >= GraphSymbols.size())
      return nullptr;
    return GraphSymbols[SymIndex];
  }

  void setGraphBlock(COFFSectionIndex SecIndex, Block *B) {
    assert(!GraphBlocks[SecIndex] && "Duplicate section at index");
    GraphBlocks[SecIndex] = B;
  }

  Block *getGraphBlock(COFFSectionIndex SecIndex) const {
    if (SecIndex <= 0 || SecIndex >= GraphSymbols.size())
      return nullptr;
    return GraphBlocks[SecIndex];
  }

  object::COFFObjectFile::section_iterator_range sections() const {
    return Obj.sections();
  }

  /// Traverse all matching relocation records in the given section. The handler
  /// function Func should be callable with this signature:
  ///   Error(const COFF::relocation&,
  ///         const object::SectionRef&, Section &)
  ///
  template <typename RelocHandlerFunction>
  Error forEachRelocation(const object::SectionRef &RelSec,
                          RelocHandlerFunction &&Func,
                          bool ProcessDebugSections = false);

  /// Traverse all matching relocation records in the given section. Convenience
  /// wrapper to allow passing a member function for the handler.
  ///
  template <typename ClassT, typename RelocHandlerMethod>
  Error forEachRelocation(const object::SectionRef &RelSec, ClassT *Instance,
                          RelocHandlerMethod &&Method,
                          bool ProcessDebugSections = false) {
    return forEachRelocation(
        RelSec,
        [Instance, Method](const auto &Rel, const auto &Target, auto &GS) {
          return (Instance->*Method)(Rel, Target, GS);
        },
        ProcessDebugSections);
  }

private:
  static uint64_t getSectionAddress(const object::COFFObjectFile &Obj,
                                    const object::coff_section *Section);
  static uint64_t getSectionSize(const object::COFFObjectFile &Obj,
                                 const object::coff_section *Section);
  static bool isComdatSection(const object::coff_section *Section);
  static unsigned getPointerSize(const object::COFFObjectFile &Obj);
  static support::endianness getEndianness(const object::COFFObjectFile &Obj);

  struct WeakAliasRequest {
    COFFSymbolIndex Alias;
    COFFSymbolIndex Target;
    StringRef SymbolName;
  };

  Section &getCommonSection();
  Expected<Symbol *> createDefinedSymbol(COFFSymbolIndex SymIndex,
                                         StringRef SymbolName,
                                         object::COFFSymbolRef Symbol,
                                         const object::coff_section *Section);

  const object::COFFObjectFile &Obj;
  std::unique_ptr<LinkGraph> G;

  Section *CommonSection = nullptr;
  std::vector<Block *> GraphBlocks;
  std::vector<Symbol *> GraphSymbols;
  std::vector<Optional<std::pair<COFFSymbolIndex, Linkage>>> ComdatLeaders;
  std::vector<WeakAliasRequest> WeakAliases;
};

template <typename RelocHandlerFunction>
Error COFFLinkGraphBuilder::forEachRelocation(const object::SectionRef &RelSec,
                                              RelocHandlerFunction &&Func,
                                              bool ProcessDebugSections) {

  auto COFFRelSect = Obj.getCOFFSection(RelSec);

  // Target sections have names in valid ELF object files.
  Expected<StringRef> Name = Obj.getSectionName(COFFRelSect);
  if (!Name)
    return Name.takeError();
  LLVM_DEBUG(dbgs() << "  " << *Name << ":\n");

  // Lookup the link-graph node corresponding to the target section name.
  auto *BlockToFix = getGraphBlock(RelSec.getIndex() + 1);
  if (!BlockToFix)
    return make_error<StringError>(
        "Referencing a section that wasn't added to the graph: " + *Name,
        inconvertibleErrorCode());

  // Let the callee process relocation entries one by one.
  for (const auto &R : RelSec.relocations())
    if (Error Err = Func(R, RelSec, *BlockToFix))
      return Err;

  LLVM_DEBUG(dbgs() << "\n");
  return Error::success();
}

} // end namespace jitlink
} // end namespace llvm

#endif // LIB_EXECUTIONENGINE_JITLINK_COFFLINKGRAPHBUILDER_H
