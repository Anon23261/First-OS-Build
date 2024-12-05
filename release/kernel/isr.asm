; ISR and IRQ handlers
section .text
global isr0
global isr1
global irq0
global irq1
global idt_load
global isr_common_stub
global irq_common_stub

extern isr_handler
extern irq_handler

; Load IDT
idt_load:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; Get pointer argument
    lidt [eax]          ; Load IDT
    pop ebp
    ret

; ISR handlers
isr0:
    cli
    push byte 0     ; Push dummy error code
    push byte 0     ; Push interrupt number
    jmp isr_common_stub

isr1:
    cli
    push byte 0     ; Push dummy error code
    push byte 1     ; Push interrupt number
    jmp isr_common_stub

; IRQ handlers
irq0:
    cli
    push byte 0     ; Push dummy error code
    push byte 32    ; Push interrupt number
    jmp irq_common_stub

irq1:
    cli
    push byte 0     ; Push dummy error code
    push byte 33    ; Push interrupt number
    jmp irq_common_stub

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
    
    call isr_handler
    
    pop eax         ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa            ; Restore registers
    add esp, 8      ; Clean up error code and interrupt number
    sti             ; Re-enable interrupts
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
    
    call irq_handler
    
    pop eax         ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa            ; Restore registers
    add esp, 8      ; Clean up error code and interrupt number
    sti             ; Re-enable interrupts
    iret            ; Return from interrupt
