#include "cpu.h"

#include <lib/assert.h>
#include <lib/print.h>
#include <stdint.h>

namespace {

platform::cpu::cpu_data bsp_data = {&bsp_data};

void reload_segments(const platform::cpu::gdtr* gdtr) {
  asm volatile(
    "lgdt %0\n"
    "push $0x8\n"
    "lea 1f(%%rip), %%rax\n"
    "push %%rax\n"
    "lretq\n"
    "1:\n"
    "mov $0x10, %%ax\n"
    "mov %%ax, %%ds\n"
    "mov %%ax, %%es\n"
    "mov %%ax, %%fs\n"
    "mov %%ax, %%gs\n"
    "mov %%ax, %%ss\n"
    :
    : "m"(*gdtr)
    : "rax", "memory");
}

void common_early_init(platform::cpu::cpu_data* cpu) {
  cpu->gdt_entries[0] = {0x0, 0x0, 0x0, 0x0};       // Null segment descriptor
  cpu->gdt_entries[1] = {0x0, 0xfffff, 0x9a, 0x2};  // 64-bit code segment descriptor
  cpu->gdt_entries[2] = {0x0, 0xfffff, 0x92, 0x0};  // 64-bit data segment descriptor

  cpu->lapic_id = 0;  // @todo: Read the LAPIC ID
  cpu->lapic_addr = platform::cpu::rdmsr(IA32_APIC_BASE) & ~0xfffULL;
}

// void common_init_full(platform::cpu::cpu_data* cpu) {
//   // @todo: Initialize the TSS, the TSS GDT entry
// }

}  // namespace

platform::cpu::cpu_data& platform::cpu::get_cpu_data() {
  cpu_data* result;
  asm volatile("mov %%gs:0x0, %0"
               : "=r"(result)
               : "m"(*reinterpret_cast<void**>(cpu_data::get_self_offset())));
  return *result;
}

void platform::cpu::init_features() {
  // Make sure that SSE and SSE2 extensions are supported
  const auto cpuid_result = cpuid(0x1, 0x0);

  if ((cpuid_result.edx & (1 << 25)) == 0 || (cpuid_result.edx & (1 << 26)) == 0)
    kpanic("SSE/SSE2 extension not supported\n");

  // Given that SSE and SSE2 are supported, we can assume that FXSAVE
  // and FXRSTOR are supported as well, the assert is just for sanity
  kassert((cpuid_result.edx & (1 << 24)) != 0);

  // Enable SSE and SSE2 extensions
  auto cr4 = read_cr4();

  cr4 |= 1 << 9;   // CR4.OSFXSR
  cr4 |= 1 << 10;  // CR4.OSXMMEXCPT

  write_cr4(cr4);
}

void platform::cpu::init_bsp() {
  common_early_init(&bsp_data);

  // Load the GDT and reload the segment registers
  const gdtr gdtr = {
    .limit = sizeof(bsp_data.gdt_entries) - 1,
    .base = reinterpret_cast<uint64_t>(&bsp_data.gdt_entries),
  };

  reload_segments(&gdtr);

  // @todo: Find a better place for this
  platform::cpu::wrmsr(IA32_GS_BASE, reinterpret_cast<uint64_t>(&bsp_data));
  platform::cpu::wrmsr(IA32_KERNEL_GS_BASE, reinterpret_cast<uint64_t>(&bsp_data));
}
