#pragma once

#include <stdint.h>

// @note: This is slightly overdone, but I think it may be useful in the future.

namespace platform::cpu {

struct __attribute__((packed)) gdtr {
  uint16_t limit;
  uint64_t base;
};

struct gdt_entry {
  constexpr gdt_entry() = default;
  constexpr gdt_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
      : limit_low16{static_cast<uint16_t>(limit & 0xffff)},
        base_low16{static_cast<uint16_t>(base & 0xffff)},
        base_middle8{static_cast<uint8_t>((base >> 16) & 0xff)},
        access{access},
        flags_and_limit_high4{static_cast<uint8_t>((flags << 4) | ((limit >> 16) & 0x0f))},
        base_high8{static_cast<uint8_t>((base >> 24) & 0xff)} {
  }

  uint16_t limit_low16;
  uint16_t base_low16;
  uint8_t base_middle8;
  uint8_t access;
  uint8_t flags_and_limit_high4;
  uint8_t base_high8;
};

struct gdt_entry_extended : gdt_entry {
  constexpr gdt_entry_extended() = default;
  constexpr gdt_entry_extended(uint64_t base, uint32_t limit, uint8_t access, uint8_t flags)
      : gdt_entry{static_cast<uint32_t>(base & 0xffffffff), limit, access, flags},
        base_high32{static_cast<uint32_t>(base >> 32)},
        reserved{0} {
  }

  uint32_t base_high32;
  uint32_t reserved;
};

}  // namespace platform::cpu
