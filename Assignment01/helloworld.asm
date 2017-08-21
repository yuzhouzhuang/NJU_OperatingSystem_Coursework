section .text ;Code Section
global main:
main:
mov eax, 4 ;Using int 80h to implement write() sys_call
mov ebx, 1
mov ecx, string
mov edx, length
int 80h
;Exit System Call
mov eax, 1
mov ebx, 0
int 80h

section .data
string: db 'Hello World', 0Ah
length: equ $-string
