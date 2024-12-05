; Declare constants for the multiboot header
MBALIGN     equ 1 << 0            ; align loaded modules on page boundaries
MEMINFO     equ 1 << 1            ; provide memory map
VIDINFO     equ 1 << 2            ; provide video mode info
FLAGS       equ MBALIGN | MEMINFO | VIDINFO
MAGIC       equ 0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)  ; checksum of above to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd 0, 0, 0, 0, 0     ; unused fields
    dd 0                 ; graphics mode
    dd 80, 25, 0        ; width, height, depth (text mode)

; Allocate the initial stack
section .bootstrap_stack, nobits
align 16
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

; Declare constants for the GDT
KERNEL_CODE_SEG equ 0x08
KERNEL_DATA_SEG equ 0x10

section .text
global _start
extern kernel_main
extern _init
extern _fini

; Error handling macro
%macro HALT_WITH_ERROR 0
    cli                  ; Disable interrupts
    hlt                 ; Halt the CPU
    jmp $              ; If we wake up, halt again
%endmacro

_start:
    ; Load multiboot info
    cmp eax, 0x2BADB002     ; Verify we were loaded by a Multiboot bootloader
    jne .no_multiboot
    
    ; Initialize the stack
    mov esp, stack_top
    
    ; Clear EFLAGS
    push 0
    popf
    
    ; Initialize essential CPU features
    ; Disable interrupts
    cli
    
    ; Load GDT
    lgdt [gdt_descriptor]
    
    ; Reload segments
    jmp KERNEL_CODE_SEG:.reload_segments
.reload_segments:
    mov ax, KERNEL_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Clear the screen
    mov edi, 0xB8000       ; VGA text buffer
    mov ecx, 2000          ; 80*25 characters
    mov ax, 0x0720         ; Black background, white foreground, space character
    rep stosw
    
    ; Reset cursor position
    mov dx, 0x3D4
    mov al, 0x0F
    out dx, al
    inc dx
    xor al, al
    out dx, al
    dec dx
    mov al, 0x0E
    out dx, al
    inc dx
    xor al, al
    out dx, al
    
    ; Call global constructors
    call _init
    
    ; Call the kernel
    push ebx              ; Pass multiboot info structure
    call kernel_main
    
    ; Call global destructors
    call _fini
    
    ; If kernel returns, halt the CPU
    cli
.hang:
    hlt
    jmp .hang

.no_multiboot:
    ; Print "ERR: No Multiboot" to video memory
    mov dword [0xB8000], 0x4F524F45 ; "ER"
    mov dword [0xB8004], 0x4F3A4F52 ; "R:"
    mov dword [0xB8008], 0x4F4F4F20 ; " NO"
    mov dword [0xB800C], 0x4F424F4D ; " MB"
    HALT_WITH_ERROR

; Global Descriptor Table
section .data
align 8
gdt:
    ; Null descriptor
    dq 0
    
    ; Kernel code segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0         ; Base (bits 0-15)
    db 0         ; Base (bits 16-23)
    db 10011010b ; Access byte - Present, Ring 0, Code Segment, Executable, Direction 0, Readable
    db 11001111b ; Flags + Limit (bits 16-19) - 4K blocks, 32-bit protected mode, not 64-bit
    db 0         ; Base (bits 24-31)
    
    ; Kernel data segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0         ; Base (bits 0-15)
    db 0         ; Base (bits 16-23)
    db 10010010b ; Access byte - Present, Ring 0, Data Segment, Direction 0, Writable
    db 11001111b ; Flags + Limit (bits 16-19) - 4K blocks, 32-bit protected mode, not 64-bit
    db 0         ; Base (bits 24-31)

gdt_descriptor:
    dw $ - gdt - 1  ; GDT size (16-bit)
    dd gdt          ; GDT address (32-bit)
