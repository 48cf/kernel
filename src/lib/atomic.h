#pragma once

#include <type_traits>

namespace kernel {

enum memory_ordering {
  kAtomicRelaxed,
  kAtomicConsume,
  kAtomicAcquire,
  kAtomicRelease,
  kAtomicAcqRel,
  kAtomicSeqCst,
};

template <typename T>
  requires std::is_integral_v<T> || std::is_pointer_v<T>
struct atomic {
  constexpr atomic() : value_{} {
  }

  constexpr atomic(T value) : value_{value} {
  }

  void store(T value, memory_ordering order = kAtomicSeqCst) {
    __atomic_store_n(&value_, value, static_cast<int>(order));
  }

  T load(memory_ordering order = kAtomicSeqCst) const {
    return __atomic_load_n(&value_, static_cast<int>(order));
  }

  T exchange(T value, memory_ordering order = kAtomicSeqCst) {
    return __atomic_exchange_n(&value_, value, static_cast<int>(order));
  }

  T fetch_add(T value, memory_ordering order = kAtomicSeqCst) {
    return __atomic_fetch_add(&value_, value, static_cast<int>(order));
  }

  T fetch_sub(T value, memory_ordering order = kAtomicSeqCst) {
    return __atomic_fetch_sub(&value_, value, static_cast<int>(order));
  }

  T fetch_and(T value, memory_ordering order = kAtomicSeqCst) {
    return __atomic_fetch_and(&value_, value, static_cast<int>(order));
  }

  T fetch_or(T value, memory_ordering order = kAtomicSeqCst) {
    return __atomic_fetch_or(&value_, value, static_cast<int>(order));
  }

  T fetch_xor(T value, memory_ordering order = kAtomicSeqCst) {
    return __atomic_fetch_xor(&value_, value, static_cast<int>(order));
  }

  T operator++() {
    return __atomic_add_fetch(&value_, 1, static_cast<int>(kAtomicSeqCst));
  }

  T operator--() {
    return __atomic_sub_fetch(&value_, 1, static_cast<int>(kAtomicSeqCst));
  }

  T operator++(int) {
    return fetch_add(1);
  }

  T operator--(int) {
    return fetch_sub(1);
  }

  T operator+=(T value) {
    return __atomic_add_fetch(&value_, value, static_cast<int>(kAtomicSeqCst));
  }

  T operator-=(T value) {
    return __atomic_sub_fetch(&value_, value, static_cast<int>(kAtomicSeqCst));
  }

  T operator&=(T value) {
    return __atomic_and_fetch(&value_, value, static_cast<int>(kAtomicSeqCst));
  }

  T operator|=(T value) {
    return __atomic_or_fetch(&value_, value, static_cast<int>(kAtomicSeqCst));
  }

  T operator^=(T value) {
    return __atomic_xor_fetch(&value_, value, static_cast<int>(kAtomicSeqCst));
  }

 private:
  T value_;
};

}  // namespace kernel
