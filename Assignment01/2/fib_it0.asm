section .data
SEP: db '************************', 0ah
SEP_LEN: equ $-SEP
PRT: db '0S_Lab_1 Fibonacci', 0ah
PRT_LEN: equ $-PRT
MSG: db 'please input n: ', 0
MSG_LEN: equ $-MSG
RES: db 'Fibonacci Sequence is: ', 0
RES_LEN: equ $-RES
SPA: db ' ', 0
SPA_LEN: equ $-SPA
ETR: db '', 0ah
ETR_LEN: equ $-ETR

section .bss
num: resb 1
fir: resb 1
sec: resb 1
cur: resb 1


section .text
global main:

main:
	mov eax, 4
	mov ebx, 0
	mov ecx, SEP
	mov edx, SEP_LEN
	int 80h ;print seperator
	
	mov eax, 4
	mov ebx, 0
	mov ecx, PRT
	mov edx, PRT_LEN
	int 80h ;print title
	
	mov eax, 4
	mov ebx, 0
	mov ecx, MSG
	mov edx, MSG_LEN
	int 80h ;prompt input
	
	mov eax, 3
	mov ebx, 0
	mov ecx, num
	mov edx, 1
	int 80h ;input number
	mov byte[fir], 0
	mov byte[sec], 1 ;initiate temporary variables
	sub byte[num], 48 ;get pure number
	
	mov eax, 4
	mov ebx, 0
	mov ecx, RES
	mov edx, RES_LEN
	int 80h ;show result
	
loop:
	cmp byte[num], 0 ;judge number
	jz finish ;finish calculation
	mov al, byte[sec]
	mov byte[cur], al
	
	mov eax, 4
	mov ebx, 1
	add byte[cur], 48
	mov ecx, cur
	mov edx, 255
	int 80h ;print current number
	
	sub byte[cur], 48
	mov eax, 4
	mov ebx, 0
	mov ecx, SPA
	mov edx, 1
	int 80h ;print a space
	
	mov al, byte[fir]
	add byte[cur], al
	mov al, byte[sec]
	mov byte[fir], al
	mov al, byte[cur]
	mov byte[sec], al
	sub byte[num], 1
	jmp loop

finish:
	mov eax, 4
	mov ebx, 0
	mov ecx, ETR
	mov edx, ETR_LEN
	int 80h ;new line
	
	mov eax, 4
	mov ebx, 0
	mov ecx, SEP
	mov edx, SEP_LEN
	int 80h ;print seperator
	
exit:
	mov eax, 1
	mov ebx, 0
	int 80h ;system exit
	
