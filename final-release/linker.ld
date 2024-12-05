/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
    /* Begin putting sections at 1 MiB, a conventional place for kernels to be
       loaded at by the bootloader. */
    . = 1M;

    _kernel_start = .;

    /* Multiboot header must be early in the image */
    .text BLOCK(4K) : ALIGN(4K)
    {
        KEEP(*(.multiboot))
        *(.text.boot)
        *(.text)
        *(.text.*)
        
        /* Exception handling */
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.gcc_except_table)
    }

    /* Read-only data */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    /* C++ static constructors/destructors (global) */
    .init_array ALIGN(4K) :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
    }
    
    .fini_array ALIGN(4K) :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
        KEEP (*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        _data_start = .;
        *(.data)
        *(.data.*)
        _data_end = .;
    }

    /* Read-write data (uninitialized) */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        _bss_end = .;
    }

    /* Stack */
    .stack BLOCK(4K) : ALIGN(4K)
    {
        _stack_bottom = .;
        . = . + 64K;  /* 64KB stack */
        _stack_top = .;
    }

    _kernel_end = .;

    /* Debugging information */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    .debug_abbrev  0 : { *(.debug_abbrev) }
    .debug_info    0 : { *(.debug_info) }
    .debug_line    0 : { *(.debug_line) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    .debug_aranges 0 : { *(.debug_aranges) }
    .debug_ranges  0 : { *(.debug_ranges) }
    .debug_str     0 : { *(.debug_str) }
    .debug_loc     0 : { *(.debug_loc) }
    .debug_macinfo 0 : { *(.debug_macinfo) }
    .debug_frame   0 : { *(.debug_frame) }

    /DISCARD/ :
    {
        *(.note.GNU-stack)
        *(.note.gnu.build-id)
        *(.comment)
    }
}
