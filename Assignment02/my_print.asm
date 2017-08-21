GLOBAL my_print

	section .text
my_print:
    push rbp
    mov rbp, rsp
    mov r8, rdi
    mov rax, 4
    mov rdi, 1
    mov rsi, r8
    mov rdx, 13
    syscall
    mov esp,ebp 
	pop ebp
    ret