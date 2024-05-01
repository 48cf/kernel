#pragma once

#include "print.h"

#define kassert(expr)                          \
  do {                                         \
    if (!(expr))                               \
      kpanic("assertion failed: %s\n", #expr); \
  } while (0)

#define kassert_msg(expr, format, ...)                         \
  do {                                                         \
    if (!(expr))                                               \
      kpanic("assertion failed: " format "\n", ##__VA_ARGS__); \
  } while (0)
