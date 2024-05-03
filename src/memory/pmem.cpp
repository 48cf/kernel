#include <lib/list.h>
#include <lib/misc.h>
#include <lib/print.h>
#include <lib/spinlock.h>
#include <limine.h>
#include <memory/pmem.h>

#include <cstring>

using namespace kernel;

// @todo: Add locking to the allocator
// @idea: Implement buddy system for physical memory allocation so we can hand out more than one
// contiguous physical page at a time
// @idea: It would be nice to implement a checksum for the physical memory database to detect memory
// corruption and panic if it happens
// @idea: Implement a separate list for dirty pages that just got freed so we can zero them out
// before putting them back in the free list, this would be done by a separate, low-priority thread

namespace {

struct physical_region {
  list_node<physical_region> node;  // Points to physical_regions
  physical_addr base;               // The base address of the physical region
  size_t page_count;                // The number of pages in the physical region
  physical_page pages[];            // The page structures for the physical region (inline)
};

spin_lock allocator_lock;
list<physical_page, &physical_page::node> free_pages;
list<physical_region, &physical_region::node> physical_regions;

volatile limine_hhdm_request hhdm_request = {
  .id = LIMINE_HHDM_REQUEST,
  .revision = 1,
  .response = nullptr,
};

volatile limine_memmap_request memmap_request = {
  .id = LIMINE_MEMMAP_REQUEST,
  .revision = 1,
  .response = nullptr,
};

void track_region(physical_addr base, size_t length) {
  size_t page_count = length / PAGE_SIZE;
  size_t aligned_length = page_count * PAGE_SIZE;
  size_t used = align_up(sizeof(physical_region) + sizeof(physical_page) * page_count, PAGE_SIZE);

  // Check if the physical region is big enough to hold the physical region structure
  // and at least 16 individual pages, I don't think it's worth bothering with such
  // small regions
  if (aligned_length - used < PAGE_SIZE * 16) {
    kprintf("pmem: skipping physical region 0x%zx-0x%zx, too small\n", base, base + aligned_length);
    return;
  }

  // Set up the physical region structure
  auto* region = static_cast<physical_region*>(P2V(base));
  region->base = base;
  region->page_count = page_count;

  kprintf("pmem: registering physical region 0x%zx-0x%zx, %zu KiB, %zu pages, %zu KiB for PFDB\n",
          region->base, region->base + aligned_length, aligned_length / 1024, region->page_count,
          used / 1024);

  // Initialize the individual page structures
  for (size_t j = 0; j < region->page_count; ++j) {
    region->pages[j].addr = region->base + j * PAGE_SIZE;
  }

  for (size_t j = 0; j < region->page_count; ++j) {
    auto& page = region->pages[j];

    if (j < used / PAGE_SIZE) {
      // Mark the pages used by the physical region structure as used
      page.usage = kPageUsagePfnDatabase;
    } else {
      // Mark the remaining pages as free
      page.usage = kPageUsageFree;
      page.is_dirty = true;

      free_pages.push_back(&page);
    }
  }

  // Add the physical region to the list
  physical_regions.push_back(region);
}

}  // namespace

void kernel::init_phys_allocator() {
  const auto* hhdm_response = hhdm_request.response;
  const auto* memmap_response = memmap_request.response;

  // @todo: Fix this lol
  kassert(hhdm_response->offset == HHDM_OFFSET);

  for (size_t i = 0; i < memmap_response->entry_count; ++i) {
    const auto* entry = memmap_response->entries[i];

    // Skip memory entries that were reported as not usable
    if (entry->type != LIMINE_MEMMAP_USABLE)
      continue;

    track_region(entry->base, entry->length);
  }
}

physical_addr kernel::alloc_page(page_usage usage) {
  kassert(usage > kPageUsageFree && usage < kPageUsageMax);

  lock_guard _(allocator_lock);

  if (free_pages.empty())
    return 0;

  auto* page = free_pages.pop_front();

  // Make sure the page is actually free, otherwise panic
  // Hopefully that never happens, that would indicate that there's a bug
  // or that the physical memory database was somehow corrupted (bad)
  kassert_msg(page->usage == kPageUsageFree, "alloc_page: page database corruption: 0x%zx usage %u",
              page->addr, page->usage);

  // Mark the page as used
  page->usage = usage;

  // If the page is marked as dirty, zero it out before handing it out
  // This flag is currently never reset, but in the future it will be used
  // to efficiently zero out pages in the background
  if (page->is_dirty) {
    auto* mem8 = static_cast<unsigned char*>(P2V(page->addr));
    std::memset(mem8, 0, PAGE_SIZE);
  }

  return page->addr;
}

void kernel::free_page(physical_addr addr) {
  auto* page = addr_to_physical_page(addr);

  // Make sure that the page comes from a registered physical region
  kassert_msg(page != nullptr, "free_page: attempt to free untracked memory at 0x%zx\n", addr);

  // Make sure that we are not trying to double-free the page, this is obviously a bug
  kassert_msg(page->usage != kPageUsageFree,
              "free_page: attempt to free memory that is already free at 0x%zx\n", addr);

  // Mark the page as free, dirty, and put it back in the free list
  // In the future we will have a separate list for dirty pages that just got freed
  page->usage = kPageUsageFree;
  page->is_dirty = true;

  lock_guard _(allocator_lock);
  free_pages.push_back(page);
}

physical_page* kernel::addr_to_physical_page(physical_addr addr) {
  for (auto* it = physical_regions.head(); it != nullptr; it = it->node.next) {
    if (addr < it->base || addr >= it->base + it->page_count * PAGE_SIZE)
      continue;

    return &it->pages[(addr - it->base) / PAGE_SIZE];
  }

  return nullptr;
}
