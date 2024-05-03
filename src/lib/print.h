#pragma once

#include <stdarg.h>
#include <stdint.h>

#include <source_location>

// I got these off some table on SO, not sure if the bright colors are correct.
// I also don't exactly use these anywhere, at least not yet.
// https://stackoverflow.com/a/33206814

#define ANSI_RESET "\033[0m"
#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"
#define ANSI_BRIGHT_BLACK "\033[90m"
#define ANSI_BRIGHT_RED "\033[91m"
#define ANSI_BRIGHT_GREEN "\033[92m"
#define ANSI_BRIGHT_YELLOW "\033[93m"
#define ANSI_BRIGHT_BLUE "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN "\033[96m"
#define ANSI_BRIGHT_WHITE "\033[97m"

namespace kernel {

void init_fb_console(uint32_t* fb, uint32_t width, uint32_t height, uint32_t pitch);

void kputc(char ch);
void kprintf(const char* format, ...);
void kvprintf(const char* format, va_list args);
void __attribute__((noreturn)) kpanic_impl(std::source_location location, const char* format, ...);

}  // namespace kernel

#define kpanic(...) kernel::kpanic_impl(std::source_location::current(), __VA_ARGS__)
