#pragma once

#include <lib/atomic.h>

namespace kernel {

struct spin_lock {
  spin_lock() = default;

  inline bool try_lock() {
    return !flag_.exchange(true, kAtomicAcquire);
  }

  inline void lock() {
    while (!try_lock()) {
      asm volatile("pause");
    }
  }

  inline void unlock() {
    flag_.store(false, kAtomicRelease);
  }

 private:
  atomic<bool> flag_ = false;
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
