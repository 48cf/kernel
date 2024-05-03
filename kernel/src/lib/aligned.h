#pragma once

#include <lib/misc.h>

namespace kernel {

template <size_t Size, size_t Align>
struct aligned_storage {
  aligned_storage() : data_{0} {
  }

  char* data() {
    return data_;
  }

  const char* data() const {
    return data_;
  }

 private:
  alignas(Align) char data_[Size];
};

template <typename... Ts>
using aligned_union = aligned_storage<max(sizeof(Ts)...), max(alignof(Ts)...)>;

}  // namespace kernel
