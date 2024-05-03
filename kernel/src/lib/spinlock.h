#pragma once

#include <stdatomic.h>

namespace kernel {

struct spin_lock {
  spin_lock() = default;

  inline bool try_lock() {
    return !locked_.exchange(true, std::memory_order_acquire);
  }

  inline void lock() {
    while (!try_lock()) {
      asm volatile("pause");
    }
  }

  inline void unlock() {
    locked_.store(false, std::memory_order_release);
  }

 private:
  std::atomic_bool locked_ = false;
};

template <typename T>
struct lock_guard {
  explicit lock_guard(T& lock) : lock_{&lock} {
    lock_->lock();
  }

  ~lock_guard() {
    lock_->unlock();
  }

 private:
  T* lock_;
};

}  // namespace kernel
