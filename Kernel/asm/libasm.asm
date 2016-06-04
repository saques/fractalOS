GLOBAL cpuVendor,_rax,_rbx,_rcx,_rdx,_rbp,_rsi,_rdi,_rsp
GLOBAL _r8,_r9,_r10,_r11,_r12,_r13,_r14,_r15,_cli,_sti
GLOBAL _lidt,picMasterMask,picSlaveMask,_irq00handler,_irq01handler
GLOBAL kb_read,rtc

EXTERN irqDispatcher

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

_rax:
	ret

_rbx:
	mov rax,rbx
	ret

_rcx:
	mov rax,rcx
	ret

_rdx:
	mov rax,rdx
	ret

_rbp:
	mov rax,rbp
	ret

_rsi:
	mov rax,rsi
	ret

_rdi:
	mov rax,rdi
	ret

_rsp:
	mov rax,rsp
	ret

_r8:
	mov rax,r8
	ret

_r9:
	mov rax,r9
	ret

_r10:
	mov rax,r10
	ret

_r11:
	mov rax,r11
	ret

_r12:
	mov rax,r12
	ret

_r13:
	mov rax,r13
	ret

_r14:
	mov rax,r14
	ret

_r15:
	mov rax,r15
	ret


_lidt:	
	cli	
	lidt [rdi]
	sti
	ret
	
picMasterMask:
	mov ax,[rdi]
	out 21h,al
	retn
	
picSlaveMask:
	mov ax,[rdi]
	out 0A1h,al
	retn

%macro irqHandlerMaster 1 

	mov rdi, %1
	call irqDispatcher
	
	;signal pic
	mov al, 20h
	out 20h, al

	iretq
%endMacro

_irq00handler:
	irqHandlerMaster 0
	
_irq01handler:
	irqHandlerMaster 1

_cli:
	cli
	ret
	
_sti:
	sti
	ret
	
kb_read:
	in al,64h
	test al,1
	jz .none
	mov rax,0
	in al,60h
	jmp .ret
.none:
	mov rax,0
.ret:
	ret
	
	
rtc:
	call bin_fmt
	mov rax,rdi
	out 70h,al
	in al,71h
	ret

bin_fmt:
	mov al,0bh
	out 70h,al
	in al,71h
	push rax
	mov al,0bh
	out 70h,al
	pop rax
	or al,04h
	out 71h,al
	ret

