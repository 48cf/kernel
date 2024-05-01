#include <lib/print.h>
#include <limine.h>
#include <memory/phys.h>
#include <system/cpu.h>

using namespace kernel;

namespace {

volatile limine_framebuffer_request fb_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 1,
  .response = nullptr,
};

}

extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C" __attribute__((noreturn)) void _start() {
  // Call all global constructors
  for (auto* func = __init_array; func < __init_array_end; ++func) {
    (*func)();
  }

  // Initialize the framebuffer console
  auto* fb = fb_request.response->framebuffers[0];
  init_fb_console(static_cast<uint32_t*>(fb->address), fb->width, fb->height, fb->pitch);

  platform::cpu::init_features();
  platform::cpu::init_bsp();

  memory::init_phys_allocator();

  // Perform a silly physical memory allocation test
  const auto phys1 = memory::alloc_phys_page(kPageUsageConventional);
  const auto phys2 = memory::alloc_phys_page(kPageUsageConventional);
  const auto phys3 = memory::alloc_phys_page(kPageUsageConventional);

  kprintf("phys1: 0x%zx\n", phys1);
  kprintf("phys2: 0x%zx\n", phys2);
  kprintf("phys3: 0x%zx\n", phys3);

  memory::free_phys_page(phys1);
  memory::free_phys_page(phys2);
  memory::free_phys_page(phys3);

  kprintf("reached end of _start, halting\n");

  while (true) {
    asm volatile("hlt");
  }
}
