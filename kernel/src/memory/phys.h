#pragma once

// Implementation inspired by Keyronex:
// https://github.com/Keyronex/Keyronex/blob/4507ad9f51809f3fb810299f66e22532dc4a1660/kernel/include/kdk/vm.h
// https://github.com/Keyronex/Keyronex/blob/4507ad9f51809f3fb810299f66e22532dc4a1660/kernel/vm/resident.c

#include <lib/list.h>
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define HHDM_OFFSET 0xffff'8000'0000'0000

#define PHYS_TO_PFN(addr) ((addr) >> PAGE_SHIFT)
#define PFN_TO_PHYS(pfn) ((pfn) << PAGE_SHIFT)

#define P2V(addr) reinterpret_cast<void*>((::kernel::memory::physical_addr)(addr) + HHDM_OFFSET)
#define V2P(addr) ((::kernel::memory::physical_addr)(addr) - HHDM_OFFSET)

enum page_usage : uint8_t {
  kPageUsageInvalid,
  kPageUsagePfnDatabase,
  kPageUsageFree,
  kPageUsageConventional,
  kPageUsagePageTable,
  kPageUsageMax = 0xf,
};

namespace kernel::memory {

using physical_addr = uintptr_t;

struct page {
  // Linkage: free_pages
  list_node<page> node;
  // The physical address of the page
  physical_addr addr : 52;
  // The usage of the page
  page_usage usage : 4;
  bool is_dirty : 1;
  // @note: 7 bits are unused
};

void init_phys_allocator();
physical_addr alloc_phys_page(page_usage usage);
void free_phys_page(physical_addr addr);
page* physical_to_page(physical_addr addr);

}  // namespace kernel::memory
