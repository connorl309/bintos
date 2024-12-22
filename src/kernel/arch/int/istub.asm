extern exception_handler
extern interrupt_handler

; NASM macros for the first few isrs.
; we push the vector code onto the stack so we can use it as an arg in the handler.
%macro exception_err_stub 1
exception_stub_%+%1:
    push byte %1
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
    mov eax, exception_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

%endmacro
%macro exception_no_err_stub 1
exception_stub_%+%1:
    push 0      ; maintain stack alignment
    push byte %1
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
    mov eax, exception_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

%endmacro
%macro interrupt_stub 1
isr_stub_%+%1:
    push 0      ; maintain stack alignment
    push byte %1
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
    mov eax, interrupt_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
%endmacro

; CPU exception table stub
exception_no_err_stub 0
exception_no_err_stub 1
exception_no_err_stub 2
exception_no_err_stub 3
exception_no_err_stub 4
exception_no_err_stub 5
exception_no_err_stub 6
exception_no_err_stub 7
exception_err_stub    8
exception_no_err_stub 9
exception_err_stub    10
exception_err_stub    11
exception_err_stub    12
exception_err_stub    13
exception_err_stub    14
exception_no_err_stub 15
exception_no_err_stub 16
exception_err_stub    17
exception_no_err_stub 18
exception_no_err_stub 19
exception_no_err_stub 20
exception_no_err_stub 21
exception_no_err_stub 22
exception_no_err_stub 23
exception_no_err_stub 24
exception_no_err_stub 25
exception_no_err_stub 26
exception_no_err_stub 27
exception_no_err_stub 28
exception_no_err_stub 29
exception_err_stub    30
exception_no_err_stub 31

; helper for the C funcs that initialize things
global exception_stub_table
exception_stub_table:
%assign i 0 
%rep    32 
    dd exception_stub_%+i
%assign i i+1 
%endrep

; IRQ0-15 remapped to 32-47
interrupt_stub 32
interrupt_stub 33
interrupt_stub 34
interrupt_stub 35
interrupt_stub 36
interrupt_stub 37
interrupt_stub 38
interrupt_stub 39
interrupt_stub 40
interrupt_stub 41
interrupt_stub 42
interrupt_stub 43
interrupt_stub 44
interrupt_stub 45
interrupt_stub 46
interrupt_stub 47

global isr_stub_table
isr_stub_table:
%assign i 32
%rep    16
    dd isr_stub_%+i
%assign i i+1
%endrep