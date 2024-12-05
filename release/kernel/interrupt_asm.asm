; Interrupt handling assembly code
section .text
global idt_load
global isr0
global isr1
global irq0
global irq1
extern interrupt_handler

; Load IDT
idt_load:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]  ; Get pointer to IDT
    lidt [eax]        ; Load IDT
    pop ebp
    ret

; Macro for ISRs without error code
%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        cli
        push byte 0    ; Push dummy error code
        push byte %1   ; Push interrupt number
        jmp isr_common_stub
%endmacro

; Macro for ISRs with error code
%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        cli
        push byte %1   ; Push interrupt number
        jmp isr_common_stub
%endmacro

; Macro for IRQs
%macro IRQ 2
    global irq%1
    irq%1:
        cli
        push byte 0    ; Push dummy error code
        push byte %2   ; Push interrupt number
        jmp irq_common_stub
%endmacro

; Common ISR stub
isr_common_stub:
    pusha           ; Push all registers
    mov ax, ds      ; Save data segment
    push eax
    
    mov ax, 0x10    ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call interrupt_handler
    
    pop eax         ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa            ; Restore registers
    add esp, 8      ; Clean up error code and interrupt number
    sti
    iret            ; Return from interrupt

; Common IRQ stub
irq_common_stub:
    pusha           ; Push all registers
    mov ax, ds      ; Save data segment
    push eax
    
    mov ax, 0x10    ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call interrupt_handler
    
    pop ebx         ; Restore data segment
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    
    popa            ; Restore registers
    add esp, 8      ; Clean up error code and interrupt number
    sti
    iret            ; Return from interrupt

; Define ISRs
ISR_NOERRCODE 0    ; Division by zero
ISR_NOERRCODE 1    ; Debug
ISR_NOERRCODE 2    ; Non-maskable interrupt
ISR_NOERRCODE 3    ; Breakpoint
ISR_NOERRCODE 4    ; Overflow
ISR_NOERRCODE 5    ; Bound range exceeded
ISR_NOERRCODE 6    ; Invalid opcode
ISR_NOERRCODE 7    ; Device not available
ISR_ERRCODE   8    ; Double fault
ISR_NOERRCODE 9    ; Coprocessor segment overrun
ISR_ERRCODE   10   ; Invalid TSS
ISR_ERRCODE   11   ; Segment not present
ISR_ERRCODE   12   ; Stack-segment fault
ISR_ERRCODE   13   ; General protection fault
ISR_ERRCODE   14   ; Page fault
ISR_NOERRCODE 15   ; Reserved
ISR_NOERRCODE 16   ; x87 FPU error
ISR_ERRCODE   17   ; Alignment check
ISR_NOERRCODE 18   ; Machine check
ISR_NOERRCODE 19   ; SIMD floating-point
ISR_NOERRCODE 20   ; Virtualization
ISR_NOERRCODE 21   ; Reserved
ISR_NOERRCODE 22   ; Reserved
ISR_NOERRCODE 23   ; Reserved
ISR_NOERRCODE 24   ; Reserved
ISR_NOERRCODE 25   ; Reserved
ISR_NOERRCODE 26   ; Reserved
ISR_NOERRCODE 27   ; Reserved
ISR_NOERRCODE 28   ; Reserved
ISR_NOERRCODE 29   ; Reserved
ISR_ERRCODE   30   ; Security
ISR_NOERRCODE 31   ; Reserved

; Define IRQs
IRQ 0, 32          ; Timer
IRQ 1, 33          ; Keyboard
IRQ 2, 34          ; Cascade for IRQ 8-15
IRQ 3, 35          ; COM2
IRQ 4, 36          ; COM1
IRQ 5, 37          ; LPT2
IRQ 6, 38          ; Floppy disk
IRQ 7, 39          ; LPT1
IRQ 8, 40          ; CMOS real-time clock
IRQ 9, 41          ; Free
IRQ 10, 42         ; Free
IRQ 11, 43         ; Free
IRQ 12, 44         ; PS/2 mouse
IRQ 13, 45         ; FPU
IRQ 14, 46         ; Primary ATA
IRQ 15, 47         ; Secondary ATA
