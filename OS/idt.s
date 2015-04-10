; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31

;  0: Divide By Zero Exception
_isr0:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
_isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub
    
_isr2:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr3:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr4:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr5:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr6:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr7:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

;  8: Double Fault Exception (With Error Code!)
_isr8:
    cli
    push byte 8        ; Note that we DONT push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

_isr9:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr10:
    cli
    push byte 10        ; Note that we DONT push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

_isr11:
    cli
    push byte 11        ; Note that we DONT push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

_isr12:
    cli
    push byte 12        ; Note that we DONT push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

_isr13:
    cli
    push byte 13        ; Note that we DONT push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

_isr14:
    cli
    push byte 14        ; Note that we DONT push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

_isr15:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

_isr16:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub
    
_isr17:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr18:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr19:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr20:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr21:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr22:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr23:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

_isr24:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub
    
_isr25:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr26:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr27:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr28:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr29:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr30:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr31:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file
extern _fault_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, _fault_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

global  load_idt

; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address

load_idt:
    mov     eax, [esp+4]    ; load the address of the IDT into register eax
    lidt    [eax]             ; load the IDT
    ret                     ; return to the calling function