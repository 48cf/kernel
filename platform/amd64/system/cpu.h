#pragma once

#include <kernel/cpu.h>
#include <stddef.h>
#include <stdint.h>

#include "gdt.h"

#define IA32_APIC_BASE 0x1b
#define IA32_FS_BASE 0xc0000100
#define IA32_GS_BASE 0xc0000101
#define IA32_KERNEL_GS_BASE 0xc0000102

#define RW_CONTROL_REG(reg)                           \
  inline uint64_t read_##reg() {                      \
    uint64_t value;                                   \
    asm volatile("mov %%" #reg ", %0" : "=r"(value)); \
    return value;                                     \
  }                                                   \
  inline void write_##reg(uint64_t value) {           \
    asm volatile("mov %0, %%" #reg ::"r"(value));     \
  }

namespace platform::cpu {

struct cpu_data : kernel::cpu_data {
  constexpr cpu_data(cpu_data* self) : self_(self) {
  }

 private:
  cpu_data* self_;

 public:
  static size_t get_self_offset() {
    // return __builtin_offsetof(cpu_data, self_);
    const auto* self_ = reinterpret_cast<const cpu_data*>(0x1000);
    return reinterpret_cast<size_t>(&self_->self_) - 0x1000;
  }

  uint32_t lapic_id;
  uintptr_t lapic_addr;

  gdt_entry gdt_entries[3];
  gdt_entry_extended gdt_tss_entry;
};

cpu_data& get_cpu_data();

void init_features();
void init_bsp();

RW_CONTROL_REG(cr0)
RW_CONTROL_REG(cr2)
RW_CONTROL_REG(cr3)
RW_CONTROL_REG(cr4)

struct cpuid_result {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
};

inline cpuid_result cpuid(uint32_t function, uint32_t ecx) {
  cpuid_result result;
  asm volatile("cpuid"
               : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
               : "a"(function), "c"(ecx));
  return result;
}

inline uint8_t inb(uint16_t port) {
  uint8_t value;
  asm volatile("inb %1, %0" : "=a"(value) : "d"(port));
  return value;
}

inline void outb(uint16_t port, uint8_t value) {
  asm volatile("outb %0, %1" ::"a"(value), "d"(port));
}

inline uint16_t inw(uint16_t port) {
  uint16_t value;
  asm volatile("inw %1, %0" : "=a"(value) : "d"(port));
  return value;
}

inline void outw(uint16_t port, uint16_t value) {
  asm volatile("outw %0, %1" ::"a"(value), "d"(port));
}

inline uint32_t inl(uint16_t port) {
  uint32_t value;
  asm volatile("inl %1, %0" : "=a"(value) : "d"(port));
  return value;
}

inline void outl(uint16_t port, uint32_t value) {
  asm volatile("outl %0, %1" ::"a"(value), "d"(port));
}

inline uint64_t rdmsr(uint32_t msr) {
  uint32_t low;
  uint32_t high;
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
  return static_cast<uint64_t>(high) << 32 | low;
}

inline void wrmsr(uint32_t msr, uint64_t value) {
  asm volatile("wrmsr" ::"a"(static_cast<uint32_t>(value)), "d"(value >> 32), "c"(msr));
}

}  // namespace platform::cpu
