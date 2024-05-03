#pragma once

#include <lib/aligned.h>
#include <lib/assert.h>

#include <new>
#include <utility>

namespace kernel {

struct nullopt_t {};

inline nullopt_t nullopt;

template <typename T>
struct optional {
  optional() = default;

  optional(nullopt_t) {
  }

  optional(const T& value) {
    new (storage_.data()) T(value);
    has_value_ = true;
  }

  optional(T&& value) {
    new (storage_.data()) T(std::move(value));
    has_value_ = true;
  }

  optional(const optional& other) : has_value_{other.has_value_} {
    if (has_value_)
      new (storage_.data()) T(other.value());
  }

  optional(optional&& other) : has_value_{other.has_value_} {
    if (has_value_)
      new (storage_.data()) T(std::move(other.value()));
  }

  optional& operator=(const optional& other) {
    if (other.has_value_) {
      if (has_value_)
        value() = other.value();
      else
        new (storage_.data()) T(other.value());

      has_value_ = true;
    } else {
      reset();
    }

    return *this;
  }

  optional& operator=(optional&& other) {
    if (other.has_value_) {
      if (has_value_)
        value() = std::move(other.value());
      else
        new (storage_.data()) T(std::move(other.value()));

      has_value_ = true;
    } else {
      reset();
    }

    return *this;
  }

  ~optional() {
    reset();
  }

  operator bool() const {
    return has_value_;
  }

  bool has_value() const {
    return has_value_;
  }

  T& operator*() {
    return value();
  }

  const T& operator*() const {
    return value();
  }

  T* operator->() {
    return &value();
  }

  const T* operator->() const {
    return &value();
  }

  T& value() {
    kassert(has_value_);
    return *std::launder(reinterpret_cast<T*>(storage_.data()));
  }

  const T& value() const {
    kassert(has_value_);
    return *std::launder(reinterpret_cast<const T*>(storage_.data()));
  }

  void reset() {
    if (has_value_) {
      value().~T();
      has_value_ = false;
    }
  }

 private:
  aligned_storage<sizeof(T), alignof(T)> storage_;
  bool has_value_ = false;
};

}  // namespace kernel
