#include <lib/print.h>

#include "cpu.h"

void kernel::kputc(char ch) {
  // @todo: Add support for serial output
  platform::cpu::outb(0xe9, ch);
}
