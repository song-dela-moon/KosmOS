/**
 * @file paging.hpp
 *
 * A collection of programs for memory paging.
 */

#pragma once

#include <cstddef>

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
