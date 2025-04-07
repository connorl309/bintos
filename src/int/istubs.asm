extern exception_handler

; NASM macros for the first few isrs.
; we push the vector code onto the stack so we can use it as an arg in the handler.
%macro exception_err_stub 1
exception_stub_%+%1:
    push byte %1
    ; for some reason 64 bit doesnt support pusha/popa so we gotta do it manually,
    ; as well as the extended registers for 64 bit. (eventually, don't think I need them rn...?)
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    ; I'm gonna go crazy, where the fuck is this argument coming from?
    mov r15, rsp
    call exception_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 8     ; Cleans up the pushed error code and pushed ISR number
    iretq          ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

%endmacro
%macro exception_no_err_stub 1
exception_stub_%+%1:
    push 0 ; dummy to maintain align
    push byte %1
    ; for some reason 64 bit doesnt support pusha/popa so we gotta do it manually,
    ; as well as the extended registers for 64 bit. (eventually, don't think I need them rn...?)
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    ; I'm gonna go crazy, where the fuck is this argument coming from?
    ; Some goofy compiler shit screws the stack pointer even with correct
    ; cflags. I'm giving up.
    mov r15, rsp
    call exception_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 8     ; Cleans up the pushed error code and pushed ISR number
    iretq          ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

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
    dq exception_stub_%+i
%assign i i+1 
%endrep
