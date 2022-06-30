//=--------- COFFLinkGraphBuilder.cpp - COFF LinkGraph builder ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Generic COFF LinkGraph buliding code.
//
//===----------------------------------------------------------------------===//
#include "COFFLinkGraphBuilder.h"

#define DEBUG_TYPE "jitlink"

static const char *CommonSectionName = "__common";

namespace llvm {
namespace jitlink {

COFFLinkGraphBuilder::COFFLinkGraphBuilder(
    const object::COFFObjectFile &Obj, Triple TT,
    LinkGraph::GetEdgeKindNameFunction GetEdgeKindName)
    : G(std::make_unique<LinkGraph>(
          Obj.getFileName().str(), Triple(std::move(TT)), getPointerSize(Obj),
          getEndianness(Obj), std::move(GetEdgeKindName))),
      Obj(Obj) {
  LLVM_DEBUG({
    dbgs() << "Created COFFLinkGraphBuilder for \"" << Obj.getFileName()
           << "\"";
  });
}

COFFLinkGraphBuilder::~COFFLinkGraphBuilder() = default;

unsigned
COFFLinkGraphBuilder::getPointerSize(const object::COFFObjectFile &Obj) {
  return Obj.is64() ? 8 : 4;
}

support::endianness
COFFLinkGraphBuilder::getEndianness(const object::COFFObjectFile &Obj) {
  return Obj.isLittleEndian() ? support::little : support::big;
}

uint64_t COFFLinkGraphBuilder::getSectionSize(const object::COFFObjectFile &Obj,
                                              const object::coff_section *Sec) {
  // Consider the difference between executable form and object form.
  // More information is inside COFFObjectFile::getSectionSize
  if (Obj.getDOSHeader())
    return std::min(Sec->VirtualSize, Sec->SizeOfRawData);
  return Sec->SizeOfRawData;
}

uint64_t
COFFLinkGraphBuilder::getSectionAddress(const object::COFFObjectFile &Obj,
                                        const object::coff_section *Section) {
  return Section->VirtualAddress + Obj.getImageBase();
}

bool COFFLinkGraphBuilder::isComdatSection(
    const object::coff_section *Section) {
  return Section->Characteristics & COFF::IMAGE_SCN_LNK_COMDAT;
}

Section &COFFLinkGraphBuilder::getCommonSection() {
  if (!CommonSection)
    CommonSection =
        &G->createSection(CommonSectionName, MemProt::Read | MemProt::Write);
  return *CommonSection;
}

Expected<std::unique_ptr<LinkGraph>> COFFLinkGraphBuilder::buildGraph() {
  if (!Obj.isRelocatableObject())
    return make_error<JITLinkError>("Object is not a relocatable COFF file");

  if (auto Err = graphifySections())
    return std::move(Err);

  if (auto Err = graphifySymbols())
    return std::move(Err);

  if (auto Err = addRelocations())
    return std::move(Err);

  return std::move(G);
}

Error COFFLinkGraphBuilder::graphifySections() {
  LLVM_DEBUG(dbgs() << "  Creating graph sections...\n");

  GraphBlocks.resize(Obj.getNumberOfSections() + 1);
  // For each section...
  for (COFFSectionIndex SecIndex = 1; SecIndex <= Obj.getNumberOfSections();
       SecIndex++) {
    Expected<const object::coff_section *> Sec = Obj.getSection(SecIndex);
    if (!Sec) {
      return Sec.takeError();
    }

    StringRef SectionName;
    if (Expected<StringRef> SecNameOrErr = Obj.getSectionName(*Sec))
      SectionName = *SecNameOrErr;

    // Avoid loading zero-sized COFF sections.
    /*bool HasContent = getSectionSize(Obj, *Sec) > 0;
    bool IsDiscardable =
        (*Sec)->Characteristics &
        (COFF::IMAGE_SCN_MEM_DISCARDABLE | COFF::IMAGE_SCN_LNK_INFO);
    dbgs() << getSectionSize(Obj, *Sec) << "\n";
    if (!HasContent || IsDiscardable) {
        LLVM_DEBUG(
            dbgs() << "    " << SecIndex << ": \"" << SectionName
            << "\" is either empty or discardable: "
            "No graph section will be created.\n"
        );
        continue;
    }*/

    // FIXME: Possibly skip debug info sections

    LLVM_DEBUG({
      dbgs() << "    "
             << "Creating section for \"" << SectionName << "\"\n";
    });

    // Get the section's memory protection flags.
    MemProt Prot = {};
    if ((*Sec)->Characteristics & COFF::IMAGE_SCN_MEM_EXECUTE)
      Prot |= MemProt::Exec;
    if ((*Sec)->Characteristics & COFF::IMAGE_SCN_MEM_READ)
      Prot |= MemProt::Read;
    if ((*Sec)->Characteristics & COFF::IMAGE_SCN_MEM_WRITE)
      Prot |= MemProt::Write;

    // Look for existing sections first.
    auto *GraphSec = G->findSectionByName(SectionName);
    if (!GraphSec)
      GraphSec = &G->createSection(SectionName, Prot);
    assert(GraphSec->getMemProt() == Prot && "MemProt should match");

    Block *B = nullptr;
    if ((*Sec)->Characteristics & COFF::IMAGE_SCN_CNT_UNINITIALIZED_DATA)
      B = &G->createZeroFillBlock(
          *GraphSec, getSectionSize(Obj, *Sec),
          orc::ExecutorAddr(getSectionAddress(Obj, *Sec)),
          (*Sec)->getAlignment(), 0);
    else {
      ArrayRef<uint8_t> Data{};
      if (auto Err = Obj.getSectionContents(*Sec, Data))
        return Err;

      B = &G->createContentBlock(
          *GraphSec,
          ArrayRef<char>(reinterpret_cast<const char *>(Data.data()),
                         Data.size()),
          orc::ExecutorAddr(getSectionAddress(Obj, *Sec)),
          (*Sec)->getAlignment(), 0);
    }

    setGraphBlock(SecIndex, B);
  }

  return Error::success();
}

Error COFFLinkGraphBuilder::graphifySymbols() {
  LLVM_DEBUG(dbgs() << "  Creating graph symbols...\n");

  ComdatLeaders.resize(Obj.getNumberOfSections() + 1);
  GraphSymbols.resize(Obj.getNumberOfSymbols());
  for (COFFSymbolIndex SymIndex = 0; SymIndex < Obj.getNumberOfSymbols();
       SymIndex++) {
    Expected<object::COFFSymbolRef> Sym = Obj.getSymbol(SymIndex);
    if (!Sym)
      return Sym.takeError();

    StringRef SymbolName;
    if (Expected<StringRef> SymNameOrErr = Obj.getSymbolName(*Sym))
      SymbolName = *SymNameOrErr;

    const COFFSectionIndex SectionIndex = Sym->getSectionNumber();
    const object::coff_section *Sec = nullptr;

    // Get section name
    StringRef SectionName;
    if (SectionIndex == COFF::IMAGE_SYM_UNDEFINED) {
      if (Sym->getValue())
        SectionName = "(common)";
      else
        SectionName = "(external)";
    } else if (SectionIndex == COFF::IMAGE_SYM_ABSOLUTE)
      SectionName = "(absolute)";
    else if (SectionIndex == COFF::IMAGE_SYM_DEBUG)
      SectionName = "(debug)";
    else {
      // Non reserved regular section number
      Expected<const object::coff_section *> SecOrErr =
          Obj.getSection(SectionIndex);
      if (SecOrErr.takeError())
        return make_error<JITLinkError>("Invalid COFF section number:" +
                                        formatv("{0:d}: ", SectionIndex));
      if (Expected<StringRef> SecNameOrErr = Obj.getSectionName(*SecOrErr))
        SectionName = *SecNameOrErr;
      Sec = *SecOrErr;
    }

    jitlink::Symbol *GSym = nullptr;
    if (Sym->isFileRecord())
      LLVM_DEBUG({
        dbgs() << "      " << SymIndex << ": Skipping FileRecord symbol \""
               << SymbolName << "\" in " << SectionName << "\n";
      });
    else if (Sym->isUndefined()) {
      LLVM_DEBUG({
        dbgs() << "      " << SymIndex
               << ": Creating external graph symbol for COFF symbol \""
               << SymbolName << "\" in " << SectionName << "\n";
      });
      GSym =
          &G->addExternalSymbol(SymbolName, Sym->getValue(), Linkage::Strong);
    } else if (Sym->isWeakExternal()) {
      const COFFSymbolIndex TagIndex =
          Sym->getAux<object::coff_aux_weak_external>()->TagIndex;
      WeakAliases.push_back({SymIndex, TagIndex, SymbolName});
    } else {
      Expected<jitlink::Symbol *> NewGSym =
          createDefinedSymbol(SymIndex, SymbolName, *Sym, Sec);
      if (!NewGSym) {
        LLVM_DEBUG({
          dbgs() << "      " << SymIndex
                 << ": Not creating graph symbol for COFF symbol \""
                 << SymbolName << "\" in " << SectionName
                 << " with an error : " << NewGSym.takeError() << "\n";
        });
        SymIndex += Sym->getNumberOfAuxSymbols();
        continue;
      }
      GSym = *NewGSym;
      if (GSym)
        LLVM_DEBUG({
          dbgs() << "      " << SymIndex
                 << ": Creating defined graph symbol for COFF symbol \""
                 << SymbolName << "\" in " << SectionName << "\n";
        });
    }

    if (GSym)
      setGraphSymbol(SymIndex, *GSym);
    SymIndex += Sym->getNumberOfAuxSymbols();
  }

  // Export the weak external symbols and alias it
  for (auto &WeakAlias : WeakAliases) {
    if (auto Target = getGraphSymbol(WeakAlias.Target)) {
      assert((llvm::count_if(G->defined_symbols(),
                             [&](const jitlink::Symbol *Sym) {
                               return Sym->getName() == WeakAlias.SymbolName;
                             }) == 0) &&
             "Duplicate defined symbol");

      Target->setLinkage(Linkage::Weak);
      Target->setScope(Scope::Default);
      Target->setName(WeakAlias.SymbolName);
      setGraphSymbol(WeakAlias.Alias, *Target);
    } else
      return make_error<JITLinkError>("Weak symbol alias requested but actual "
                                      "symbol not defined in symbol " +
                                      WeakAlias.Alias);
  }

  return Error::success();
}

Expected<Symbol *> COFFLinkGraphBuilder::createDefinedSymbol(
    COFFSymbolIndex SymIndex, StringRef SymbolName,
    object::COFFSymbolRef Symbol, const object::coff_section *Section) {
  if (Symbol.isCommon()) {
    // FIXME: correct alignment
    return &G->addCommonSymbol(SymbolName, Scope::Default, getCommonSection(),
                               orc::ExecutorAddr(), Symbol.getValue(),
                               Symbol.getValue(), false);
  }
  if (Symbol.isAbsolute())
    return &G->addAbsoluteSymbol(SymbolName,
                                 orc::ExecutorAddr(Symbol.getValue()), 0,
                                 Linkage::Strong, Scope::Local, false);

  const COFFSectionIndex SectionIndex = Symbol.getSectionNumber();
  if (llvm::COFF::isReservedSectionNumber(SectionIndex))
    return make_error<JITLinkError>(
        "Reserved section number used in regular symbol " +
        formatv("{0:d}", SymIndex));

  // When IMAGE_SCN_LNK_COMDAT flag is set in the flags of a section,
  // the section is called a COMDAT section. It contains two symbols
  // in a sequence that specifes the behavior. First symbol is the section
  // symbol which contains the size and name of the section. It also contains
  // selectino type that specifies how duplicates of the symbol is handled.
  // Second symbol is COMDAT symbol which usually defines the external name and
  // data type.
  //
  // Since two symbols always come in a specific order, we store the first
  // symbol to ComdatLeaders table and emits it as a external symbol when
  // we encounter the COMDAT symbol.
  Block *B = getGraphBlock(Symbol.getSectionNumber());
  if (Symbol.isExternal()) {
    // This is not a comdat sequence, export the symbol as it is
    if (!ComdatLeaders[Symbol.getSectionNumber()])
      return &G->addDefinedSymbol(
          *B, Symbol.getValue(), SymbolName, 0, Linkage::Strong, Scope::Default,
          Symbol.getComplexType() == COFF::IMAGE_SYM_DTYPE_FUNCTION, false);
    else {
      // This is the second symbol of a COMDAT sequence that contains the name
      // and type of the symbol.
      auto Leader = *ComdatLeaders[SectionIndex];
      const COFFSymbolIndex TargetIndex = Leader.first;
      const Linkage L = Leader.second;
      jitlink::Symbol *Target = getGraphSymbol(TargetIndex);
      assert(Target && "COMDAT leaader is invalid.");
      assert((llvm::count_if(G->defined_symbols(),
                             [&](const jitlink::Symbol *Sym) {
                               return Sym->getName() == SymbolName;
                             }) == 0) &&
             "Duplicate defined symbol");
      Target->setName(SymbolName);
      Target->setLinkage(L);
      Target->setCallable(Symbol.getComplexType() ==
                          COFF::IMAGE_SYM_DTYPE_FUNCTION);
      Target->setScope(Scope::Default);
      setGraphSymbol(SymIndex, *Target);
      return nullptr;
    }
  }

  if (Symbol.getStorageClass() == COFF::IMAGE_SYM_CLASS_STATIC) {
    const object::coff_aux_section_definition *Def =
        Symbol.getSectionDefinition();
    if (!Def || !isComdatSection(Section) || ComdatLeaders[SectionIndex]) {
      // Handle typical static symbol
      return &G->addDefinedSymbol(
          *B, Symbol.getValue(), SymbolName, 0, Linkage::Strong, Scope::Local,
          Symbol.getComplexType() == COFF::IMAGE_SYM_DTYPE_FUNCTION, false);
    }
    if (Def->Selection == COFF::IMAGE_COMDAT_SELECT_ASSOCIATIVE) {
      // Dead strip should handle this implictly
      return nullptr;
    }
    Linkage L = Linkage::Strong;
    jitlink::Symbol *GSym = &G->addAnonymousSymbol(*B, Symbol.getValue(),
                                                   Def->Length, false, false);
    switch (Def->Selection) {
    case COFF::IMAGE_COMDAT_SELECT_NODUPLICATES: {
      L = Linkage::Strong;
      break;
    }
    case COFF::IMAGE_COMDAT_SELECT_ANY: {
      L = Linkage::Weak;
      break;
    }
    case COFF::IMAGE_COMDAT_SELECT_EXACT_MATCH:
    case COFF::IMAGE_COMDAT_SELECT_SAME_SIZE: {
      // FIXME: Implement size/content validation when LinkGraph is able to
      // handle this.
      L = Linkage::Weak;
      break;
    }
    case COFF::IMAGE_COMDAT_SELECT_LARGEST: {
      // FIXME: Support IMAGE_COMDAT_SELECT_LARGEST when LinkGraph is able to
      // handle this.
      return make_error<JITLinkError>(
          "IMAGE_COMDAT_SELECT_LARGEST is not supported.");
    }
    case COFF::IMAGE_COMDAT_SELECT_NEWEST: {
      // Even link.exe doesn't support this selection properly.
      return make_error<JITLinkError>(
          "IMAGE_COMDAT_SELECT_NEWEST is not supported.");
    }
    default: {
      return make_error<JITLinkError>("Invalid comdat selection type: " +
                                      formatv("{0:d}", Def->Selection));
    }
    }
    ComdatLeaders[SectionIndex] = {SymIndex, L};
    return GSym;
  }
  return make_error<JITLinkError>("Unsupported storage class " +
                                  formatv("{0:d}", Symbol.getStorageClass()) +
                                  " in symbol " + formatv("{0:d}", SymIndex));
}
} // namespace jitlink
} // namespace llvm