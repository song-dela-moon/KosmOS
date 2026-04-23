/**
 * @file paging.hpp
 *
 * A collection of programs for memory paging.
 */

#pragma once

#include <cstddef>
#include <cstdint>

/** @brief Number of page directories to allocate statically
 *
 * This constant is used in SetupIdentityPageTable.
 * One page directory can map 512 2MiB pages, so
 * kPageDirectoryCount x 1GiB of virtual address space will be mapped.
 */
const size_t kPageDirectoryCount = 64;

/** @brief Sets up the page table so that virtual address equals physical address.
 * Eventually the CR3 register will point to the correctly set up page table.
 */
void SetupIdentityPageTable();

void InitializePaging();

union LinearAddress4Level {
  uint64_t value;

  struct {
    uint64_t offset : 12;
    uint64_t page : 9;
    uint64_t dir : 9;
    uint64_t pdp : 9;
    uint64_t pml4 : 9;
    uint64_t : 16;
  } __attribute__((packed)) parts;

  int Part(int page_map_level) const {
    switch (page_map_level) {
    case 0: return parts.offset;
    case 1: return parts.page;
    case 2: return parts.dir;
    case 3: return parts.pdp;
    case 4: return parts.pml4;
    default: return 0;
    }
  }

  void SetPart(int page_map_level, int value) {
    switch (page_map_level) {
    case 0: parts.offset = value; break;
    case 1: parts.page = value; break;
    case 2: parts.dir = value; break;
    case 3: parts.pdp = value; break;
    case 4: parts.pml4 = value; break;
    }
  }
};

union PageMapEntry {
  uint64_t data;

  struct {
    uint64_t present : 1;
    uint64_t writable : 1;
    uint64_t user : 1;
    uint64_t write_through : 1;
    uint64_t cache_disable : 1;
    uint64_t accessed : 1;
    uint64_t dirty : 1;
    uint64_t huge_page : 1;
    uint64_t global : 1;
    uint64_t : 3;

    uint64_t addr : 40;
    uint64_t : 12;
  } __attribute__((packed)) bits;

  PageMapEntry* Pointer() const {
    return reinterpret_cast<PageMapEntry*>(bits.addr << 12);
  }

  void SetPointer(PageMapEntry* p) {
    bits.addr = reinterpret_cast<uint64_t>(p) >> 12;
  }
};
