%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push    dword 0                     ; push 0 as error code
    push    dword %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push    dword %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

common_interrupt_handler:               ; the common parts of the generic interrupt handler
    ; save the registers
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esp
    push    ebp
    push    esi
    push    edi

    ; call the C function
    extern  interrupt_handler
    call    interrupt_handler

    ; restore the registers
    pop     edi
    pop     esi
    pop     ebp
    pop     esp
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    ; restore the esp
    add     esp, 8

    ; return to the code that got interrupted
    iret

no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler 8
no_error_code_interrupt_handler 9
error_code_interrupt_handler 10
error_code_interrupt_handler 11
error_code_interrupt_handler 12
error_code_interrupt_handler 13
error_code_interrupt_handler 14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19

; irqs
no_error_code_interrupt_handler 32
no_error_code_interrupt_handler 33
no_error_code_interrupt_handler 34
no_error_code_interrupt_handler 35
no_error_code_interrupt_handler 36
no_error_code_interrupt_handler 37
no_error_code_interrupt_handler 38
no_error_code_interrupt_handler 39
no_error_code_interrupt_handler 40
no_error_code_interrupt_handler 41
no_error_code_interrupt_handler 42
no_error_code_interrupt_handler 43
no_error_code_interrupt_handler 44
no_error_code_interrupt_handler 45
no_error_code_interrupt_handler 46
no_error_code_interrupt_handler 47

global  load_idt

; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address

load_idt:
    mov     eax, [esp+4]    ; load the address of the IDT into register eax
    lidt    [eax]             ; load the IDT
    ret                     ; return to the calling function


