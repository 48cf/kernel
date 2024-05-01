#include <lib/print.h>
#include <stddef.h>
#include <stdint.h>

// GCC and clang reserve the right to generate calls to these functions
// even if they are not explicitly used in the code.

extern "C" void* memcpy(void* dest, const void* src, size_t count) {
  char* dest8 = (char*)dest;
  const char* src8 = (const char*)src;

  while (count--) {
    *dest8++ = *src8++;
  }

  return dest;
}

extern "C" void* memset(void* dest, int value, size_t count) {
  unsigned char* dest8 = (unsigned char*)dest;
  unsigned char val8 = (unsigned char)value;

  while (count--) {
    *dest8++ = val8;
  }

  return dest;
}

extern "C" void* memmove(void* dest, const void* src, size_t count) {
  char* dest8 = (char*)dest;
  const char* src8 = (const char*)src;

  if (src8 > dest8) {
    while (count--) {
      *dest8++ = *src8++;
    }
  } else {
    dest8 += count;
    src8 += count;

    while (count--) {
      *--dest8 = *--src8;
    }
  }

  return dest;
}

extern "C" int memcmp(const void* ptr1, const void* ptr2, size_t count) {
  const unsigned char* p1 = (const unsigned char*)ptr1;
  const unsigned char* p2 = (const unsigned char*)ptr2;

  while (count--) {
    if (*p1 != *p2)
      return *p1 < *p2 ? -1 : 1;

    p1++;
    p2++;
  }

  return 0;
}

// These following two stubs are required by the Itanium C++ ABI

extern "C" int __cxa_atexit(void (*)(void*), void*, void*) {
  // @note: This really shouldn't be called given that we compile
  // with -fno-use-cxa-atexit
  kpanic("Attempt to call __cxa_atexit\n");
}

extern "C" void __cxa_pure_virtual() {
  kpanic("Pure virtual function called\n");
}
