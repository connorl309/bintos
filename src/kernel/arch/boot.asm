; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
VIDEO	 equ 1 << 2			; provide framebuffer info
MBFLAGS  equ  MBALIGN | MEMINFO | VIDEO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + MBFLAGS)   ; checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot_header
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM
	times 5 dd 0
	dd 0 ; set graphics mode, not text mode
	dd 1024 ; width
	dd 768 ; height
	dd 32 ; 32 bits of color per pixel (RGB)

section .bss
align 16
stack_bottom:
resb 32768 ; 32 KiB
stack_top:

; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
; Declare _start as a function symbol with the given symbol size.
section .text
global _start:function (_start.end - _start)
_start:
	cli ; disable interrupts

	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems). This is necessarily done
	; in assembly as languages such as C cannot function without a stack.
	mov esp, stack_top

	extern kernel_main
	push eax ; per GRUB, this should be the supposed Multiboot magic number
	push ebx ; per GRUB, this should be the supposed Multiboot ptr
	call kernel_main

	; We should never get here
	cli
.hang:	hlt
	jmp .hang
.end: