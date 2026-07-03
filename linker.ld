ENTRY(_start)

SECTIONS
{
    . = 0x8000;      /* Kernel load address for 32-bit ARM (kernel.img) */
    .text : { KEEP(*(.text.boot)) *(.text .text.*) }

    /DISCARD/ : { *(.ARM.exidx*) }
}
