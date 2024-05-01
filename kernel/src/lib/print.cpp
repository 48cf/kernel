#define NANOPRINTF_IMPLEMENTATION
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_VISIBILITY_STATIC

#include "print.h"

#include <backends/fb.h>
#include <flanterm.h>
#include <nanoprintf.h>
#include <system/cpu.h>

namespace {

flanterm_context* flanterm_ctx = nullptr;

void putc(int ch, void*) {
  kernel::kputc(ch);

  if (flanterm_ctx != nullptr) {
    char c = ch;
    flanterm_write(flanterm_ctx, &c, 1);
  }
}

}  // namespace

void kernel::init_fb_console(uint32_t* fb, uint32_t width, uint32_t height, uint32_t pitch) {
  flanterm_ctx = flanterm_fb_init(nullptr, nullptr, fb, width, height, pitch, 8, 16, 8, 8, 8, 0,
                                  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                  nullptr, 0, 0, 1, 0, 0, 0);
}

void kernel::kprintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  kvprintf(format, args);
  va_end(args);
}

void kernel::kvprintf(const char* format, va_list args) {
  npf_vpprintf(putc, nullptr, format, args);
}

void __attribute__((noreturn)) kernel::kpanic_impl(        //
  std::source_location location, const char* format, ...)  //
{
  va_list args;
  va_start(args, format);
  kprintf("panic: ");
  kvprintf(format, args);
  kprintf("panic: %s in %s:%u\n", location.function_name(), location.file_name(), location.line());
  va_end(args);

  asm volatile("cli");
  while (true) {
    asm volatile("hlt");
  }
}
