#pragma once

// An intrusive, doubly linked list implementation inspired by something
// I found off a quick Google search.
// https://github.com/wc-duck/intrusive_list/blob/505e5c26fe712fd638da3f6e662a32679fc38d14/list.h
//
// I've renamed some functions and implemented them myself after reading the original
// implementation, but the credit goes to the original author.

#include "assert.h"

namespace kernel {

template <typename T>
struct list_node {
  T* next = nullptr;
  T* prev = nullptr;
};

template <typename T, list_node<T> T::*m_node>
struct list {
  ~list() {
    clear();
  }

  // Inserts an item at the front of the list
  void push_front(T* item) {
    auto* node = &(item->*m_node);

    kassert(node->prev == nullptr && node->next == nullptr);

    node->prev = nullptr;
    node->next = head_;

    if (head_ != nullptr)
      (head_->*m_node).prev = item;

    if (tail_ == nullptr)
      tail_ = item;

    head_ = item;
  }

  // Inserts an item at the back of the list
  void push_back(T* item) {
    if (tail_ == nullptr) {
      push_front(item);
      return;
    }

    auto* node = &(item->*m_node);
    auto* tail_node = &(tail_->*m_node);

    kassert(node->prev == nullptr && node->next == nullptr);

    node->prev = tail_;
    node->next = nullptr;
    tail_node->next = item;
    tail_ = item;
  }

  // Inserts an item before the specified item
  void insert_before(T* item, T* before) {
    auto* node = &(item->*m_node);
    auto* before_node = &(before->*m_node);

    kassert(node->prev == nullptr && node->next == nullptr);

    if (before_node->prev != nullptr)
      (before_node->prev->*m_node).next = item;

    node->prev = before_node->prev;
    node->next = before;
    before_node->prev = item;

    if (before == head_)
      head_ = item;
  }

  // Inserts an item after the specified item
  void insert_after(T* item, T* after) {
    auto* node = &(item->*m_node);
    auto* after_node = &(after->*m_node);

    kassert(node->prev == nullptr && node->next == nullptr);

    if (after_node->next != nullptr)
      (after_node->next->*m_node).prev = item;

    node->next = after_node->next;
    node->prev = after;
    after_node->next = item;

    if (after == tail_)
      tail_ = item;
  }

  // Removes the item at the front of the list and returns it
  T* pop_front() {
    T* item = head_;
    remove(item);
    return item;
  }

  // Removes the item at the back of the list and returns it
  T* pop_back() {
    T* item = tail_;
    remove(item);
    return item;
  }

  // Removes the specified item from the list
  void remove(T* item) {
    auto* node = &(item->*m_node);
    auto* prev = node->prev;
    auto* next = node->next;

    if (item == head_)
      head_ = next;
    else if (item == tail_)
      tail_ = prev;

    if (prev != nullptr)
      (prev->*m_node).next = next;

    if (next != nullptr)
      (next->*m_node).prev = prev;

    node->next = nullptr;
    node->prev = nullptr;
  }

  // Removes all items from the list
  void clear() {
    while (head_ != nullptr) {
      pop_front();
    }
  }

  // Returns true if the list is empty
  bool empty() const {
    return head_ == nullptr;
  }

  // Returns the first item in the list
  T* head() {
    return head_;
  }

  const T* head() const {
    return head_;
  }

  // Returns the last item in the list
  T* tail() {
    return tail_;
  }

  const T* tail() const {
    return tail_;
  }

  // Returns the item after the specified item
  T* next(T* node) {
    return (node->*m_node).next;
  }

  const T* next(const T* node) const {
    return (node->*m_node).next;
  }

  // Returns the item before the specified item
  T* prev(T* node) {
    return (node->*m_node).prev;
  }

  const T* prev(const T* node) const {
    return (node->*m_node).prev;
  }

 private:
  T* head_ = nullptr;
  T* tail_ = nullptr;
};

}  // namespace kernel
