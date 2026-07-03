ENTRY(_start)

SECTIONS
{
    . = 0x8000;      /* Kernel load address for 32-bit ARM (kernel.img) */
    .text : { *(.text .text.*) }

    /DISCARD/ : { *(.ARM.exidx*) }
}
