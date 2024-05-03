#pragma once

// Inspired by frigg's variant implementation
// https://github.com/managarm/frigg/blob/667ffd661631c2707fda6bf8b1b2f6c7325e8a8a/include/frg/variant.hpp

#include <lib/aligned.h>
#include <lib/print.h>

#include <new>
#include <type_traits>
#include <utility>

namespace kernel {

namespace detail {

// check if a type T exists in a list of types Ts
template <typename T, typename... Ts>
struct exists : std::false_type {};

template <typename T, typename U, typename... Ts>
struct exists<T, U, Ts...>
    : std::conditional_t<std::is_same_v<T, U>, std::true_type, exists<T, Ts...>> {};

template <typename T, typename... Ts>
constexpr bool exists_v = exists<T, Ts...>::value;

// get the index of a type T in a list of types Ts
// returns invalid_index if T is not in Ts
static constexpr size_t invalid_index = static_cast<size_t>(-1);

template <typename T, typename... Ts>
struct index_of {
  static constexpr size_t value = invalid_index;
};

template <typename T, typename U, typename... Ts>
struct index_of<T, U, Ts...> {
  static constexpr size_t value = index_of<T, Ts...>::value + 1;
};

template <typename T, typename... Ts>
struct index_of<T, T, Ts...> {
  static constexpr size_t value = 0;
};

template <typename T, typename... Ts>
constexpr size_t index_of_v = index_of<T, Ts...>::value;

// get the type at index N in a list of types Ts
template <size_t N, typename... Ts>
struct type_at;

template <size_t N, typename T, typename... Ts>
struct type_at<N, T, Ts...> {
  using type = typename type_at<N - 1, Ts...>::type;
};

template <typename T, typename... Ts>
struct type_at<0, T, Ts...> {
  using type = T;
};

template <size_t N, typename... Ts>
using type_at_t = typename type_at<N, Ts...>::type;

}  // namespace detail

template <typename... Ts>
struct variant {
 public:
  constexpr variant() : index_{detail::invalid_index} {
  }

  template <typename T>
  variant(const T& value) : index_{detail::index_of_v<T, Ts...>} {
    new (&storage_) T(value);
  }

  template <typename T>
  variant(T&& value) : index_{detail::index_of_v<T, Ts...>} {
    new (&storage_) T(std::move(value));
  }

  variant(const variant& other) {
    copy_construct<0>(other);
  }

  variant& operator=(const variant& other) {
    if (index_ == other.index_) {
      assign<0>(other);
    } else {
      if (index_ != detail::invalid_index)
        destruct<0>();

      if (other.index_ != detail::invalid_index)
        copy_construct<0>(other);
    }
  }

  variant(variant&& other) {
    move_construct<0>(std::move(other));
  }

  variant& operator=(variant&& other) {
    if (index_ == other.index_) {
      assign<0>(std::move(other));
    } else {
      if (index_ != detail::invalid_index)
        destruct<0>();

      if (other.index_ != detail::invalid_index)
        move_construct<0>(std::move(other));
    }
  }

  ~variant() {
    if (index_ != detail::invalid_index)
      destruct<0>();
  }

  size_t index() const {
    return index_;
  }

  template <typename T>
  T& get() {
    static_assert(detail::exists_v<T, Ts...>, "T is not in Ts...");
    return *std::launder(reinterpret_cast<T*>(storage_.data()));
  }

  template <typename T>
  const T& get() const {
    static_assert(detail::exists_v<T, Ts...>, "T is not in Ts...");
    return *std::launder(reinterpret_cast<const T*>(storage_.data()));
  }

 private:
  template <size_t N>
    requires(N < sizeof...(Ts))
  void destruct() {
    using T = detail::type_at_t<N, Ts...>;

    if (index_ == N) {
      get<T>().~T();
      index_ = detail::invalid_index;
    } else {
      destruct<N + 1>();
    }
  }

  template <size_t N>
    requires(N == sizeof...(Ts))
  void destruct() {
    kpanic("variant: invalid index");
  }

  template <size_t N>
    requires(N < sizeof...(Ts))
  void copy_construct(const variant& other) {
    using T = detail::type_at_t<N, Ts...>;

    if (other.index_ == N) {
      new (&storage_) T(other.get<T>());
      index_ = N;
    } else {
      copy_construct<N + 1>(other);
    }
  }

  template <size_t N>
    requires(N == sizeof...(Ts))
  void copy_construct(const variant& other) {
    kpanic("variant: invalid index");
  }

  template <size_t N>
    requires(N < sizeof...(Ts))
  void move_construct(variant&& other) {
    using T = detail::type_at_t<N, Ts...>;

    if (other.index_ == N) {
      new (&storage_) T(std::move(other.get<T>()));
      index_ = N;
    } else {
      move_construct<N + 1>(std::move(other));
    }
  }

  template <size_t N>
    requires(N == sizeof...(Ts))
  void move_construct(variant&& other) {
    kpanic("variant: invalid index");
  }

  template <size_t N>
    requires(N < sizeof...(Ts))
  void assign(variant other) {
    using T = detail::type_at_t<N, Ts...>;

    if (index_ == N) {
      get<T>() = std::move(other.get<T>());
    } else {
      assign<N + 1>(std::move(other));
    }
  }

  template <size_t N>
    requires(N == sizeof...(Ts))
  void assign(variant other) {
    kpanic("variant: invalid index");
  }

 private:
  aligned_union<Ts...> storage_;
  size_t index_;
};

}  // namespace kernel
