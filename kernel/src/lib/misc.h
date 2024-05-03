#pragma once

#include <stddef.h>
#include <stdint.h>

namespace kernel {

template <typename T>
constexpr T align_up(T value, size_t alignment) {
  return (value + alignment - 1) & ~static_cast<T>(alignment - 1);
}

template <typename T>
constexpr T align_down(T value, size_t alignment) {
  return value & ~static_cast<T>(alignment - 1);
}

template <typename T>
constexpr const T& min(const T& a) {
  return a;
}

template <typename T>
constexpr const T& min(const T& a, const T& b) {
  return a < b ? a : b;
}

template <typename T, typename... Ts>
constexpr const T& min(const T& a, const Ts&... args) {
  return min(a, min(args...));
}

template <typename T>
constexpr const T& max(const T& a) {
  return a;
}

template <typename T>
constexpr const T& max(const T& a, const T& b) {
  return a > b ? a : b;
}

template <typename T, typename... Ts>
constexpr const T& max(const T& a, const Ts&... args) {
  return max(a, max(args...));
}

}  // namespace kernel
