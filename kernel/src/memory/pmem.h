#pragma once

// Implementation inspired by Keyronex:
// https://github.com/Keyronex/Keyronex/blob/4507ad9f51809f3fb810299f66e22532dc4a1660/kernel/include/kdk/vm.h
// https://github.com/Keyronex/Keyronex/blob/4507ad9f51809f3fb810299f66e22532dc4a1660/kernel/vm/resident.c

#include <lib/list.h>

#include <cstdint>

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define HHDM_OFFSET 0xffff'8000'0000'0000

#define P2V(addr) reinterpret_cast<void*>((::kernel::physical_addr)(addr) + HHDM_OFFSET)
#define V2P(addr) ((::kernel::physical_addr)(addr) - HHDM_OFFSET)

namespace kernel {

enum pmem_flags : uint8_t {
  kPmemNone = 0,
};

enum page_usage : uint8_t {
  kPageUsageInvalid,
  kPageUsagePfnDatabase,
  kPageUsageFree,
  kPageUsageConventional,
  kPageUsagePageTable,
  kPageUsageMax = 0xf,
};

using physical_addr = uintptr_t;

struct physical_page {
  list_node<physical_page> node;  // Points to free_pages in pmem.cpp
  struct {
    physical_addr addr : 52;  // The physical address of the page
    page_usage usage : 4;     // The usage of the page
    bool is_dirty : 1;        // Whether the page is dirty
  };  // @note: 7 bits are unused
};

void init_phys_allocator();
physical_addr alloc_page(page_usage usage);
void free_page(physical_addr addr);

physical_page* addr_to_physical_page(physical_addr addr);

}  // namespace kernel
