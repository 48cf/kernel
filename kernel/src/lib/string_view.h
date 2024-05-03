#pragma once

#include <lib/assert.h>
#include <stddef.h>
#include <stdint.h>

namespace kernel {

struct string_view {
  constexpr static size_t npos = static_cast<size_t>(-1);

  string_view() = default;

  string_view(const char* data, size_t size) : data_{data}, size_{size} {
  }

  string_view(const char* data) : data_{data} {
    while (data[size_] != '\0') {
      ++size_;
    }
  }

  template <size_t N>
  string_view(const char (&data)[N]) : data_{data}, size_{N - 1} {
  }

  string_view(const string_view&) = default;
  string_view& operator=(const string_view&) = default;

  string_view(string_view&& other) : data_{other.data_}, size_{other.size_} {
    other.data_ = nullptr;
    other.size_ = 0;
  }

  string_view& operator=(string_view&& other) {
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  const char* data() const {
    return data_;
  }

  size_t size() const {
    return size_;
  }

  bool empty() const {
    return size_ == 0;
  }

  char operator[](size_t index) const {
    kassert(index < size_);
    return data_[index];
  }

  string_view substr(size_t pos, size_t count = npos) const {
    if (count == npos)
      count = size_ - pos;

    kassert(pos <= size_);
    kassert(pos + count <= size_);

    return string_view{data_ + pos, count};
  }

  size_t find(char c, size_t start_pos = 0) const {
    for (size_t i = start_pos; i < size_; ++i) {
      if (data_[i] == c)
        return i;
    }

    return npos;
  }

 private:
  const char* data_ = nullptr;
  size_t size_ = 0;
};

}  // namespace kernel
