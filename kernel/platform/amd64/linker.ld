/* Stripped down version of the linker script that comes with limine-cpp-barebones */
/* https://github.com/limine-bootloader/limine-cpp-template/blob/77c5a76ad2ac7b03d7c440f813c3287a5802ec0d/kernel/linker.ld */

ENTRY(_start)

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

PHDRS
{
    text    PT_LOAD     FLAGS(1 << 0 | 1 << 2); /* R-X */
    rodata  PT_LOAD     FLAGS(1 << 2);          /* R-- */
    data    PT_LOAD     FLAGS(1 << 1 | 1 << 2); /* RW- */
    dynamic PT_DYNAMIC  FLAGS(1 << 1 | 1 << 2); /* Dynamic */
}

SECTIONS
{
    . = 0xffffffff80000000;

    .text : {
        *(.text .text.*)
    } :text

    . += CONSTANT(MAXPAGESIZE);

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    .init_array : {
        __init_array = .;
        *(.init_array .init_array.*)
        __init_array_end = .;
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .data : {
        *(.data .data.*)
    } :data

    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}
