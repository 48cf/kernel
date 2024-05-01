#pragma once

#include <stddef.h>
#include <stdint.h>

namespace kernel {

// Aligns the given value up to the specified alignment.
// The alignment must be a power of two.
template <typename T>
constexpr T align_up(T value, size_t alignment) {
  return (value + alignment - 1) & ~static_cast<T>(alignment - 1);
}

// Aligns the given value down to the specified alignment.
// The alignment must be a power of two.
template <typename T>
constexpr T align_down(T value, size_t alignment) {
  return value & ~static_cast<T>(alignment - 1);
}

}  // namespace kernel
