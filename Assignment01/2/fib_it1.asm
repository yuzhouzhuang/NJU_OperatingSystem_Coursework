; •	系统请求输⼊⼀个正整数，⽤户输⼊需要显⽰的项的个数，回车键结束输
; ⼊；显⽰指定数⽬的斐波那契数列项，要求各个项⽤空格隔开，并⽤不同颜⾊显⽰（不限制颜⾊的种类、数⽬）
; •	计算达到 25 项可能会超出 16 位⼆进制数，如果实现了超过 25 项的计算可以加分。
; •	⿎励使⽤⾼级指令（可加分）。
; •	如果有其他亮点，⽆论是功能丰富还是代码实现上，检查时请向助教提出，酌情进⾏加分。
; •	注意本次实验要求在 linux（建议安装32位系统，当然学长亲测64位系也是可以的）
; •	/mac/win 系统上⾯完成，⽽不是在 bochs 内。
section .data
SEP: db '************************', 0Ah
SEP_LEN: equ $-SEP
MSG1: db '0S_Lab_1 Fibonacci', 0Ah
MSG1_LEN: equ $-MSG1
MSG2: db 'please input n = '
MSG2_LEN: equ $-MSG2
MSG3: db 'Fibonacci Sequence is: ', 0Ah
MSG3_LEN: equ $-MSG3
NEWLINE: db 0Ah
NEWLINE_LEN: equ $-NEWLINE

SPA: db ' ', 0
SPA_LEN: equ $-SPA

section .bss
INPT: resb 4
num: resb 1
ass: resb 1
num1: resb 16
num2: resb 16
num3: resb 16
cf      resb    1       ; 进位  
flag_nz     resb    1       ; 非零标志位  
buffer      resb    1       ; 之前读入数字的值  
digit       resb    1       ; 读入的一位数字  
num_count   resb    1       ; 输入数据的总个数  
digit_count resb    1       ; 每个数据的位数  


section .text
GLOBAL main:

main:
	mov eax, 4
	mov ebx, 0
	mov ecx, SEP
	mov edx, SEP_LEN
	int 0x80;print '************************'

	mov eax, 4
	mov ebx, 0
	mov ecx, MSG1
	mov edx, MSG1_LEN
	int 0x80;print '0S_Lab_1 Fibonacci'

	mov eax, 4
	mov ebx, 0
	mov ecx, MSG2
	mov edx, MSG2_LEN
	int 0x80;print 'please input n: '

	mov eax, 3
	mov ebx, 0
	mov ecx, INPT
	mov edx, 4
	int 0x80;get INPUT

	mov eax, 4
	mov ebx, 0
	mov ecx, MSG3
	mov edx, MSG3_LEN
	int 0x80;print 'Fibonacci Sequence is: '

	mov byte[num], 5

    mov cx, 15  
set:  
    mov bx, num1  
    add bx, cx  
    sub bx, 1     
    mov byte[bx],0  
    mov bx, num2  
    add bx, cx  
    sub bx, 1     
    mov byte[bx],0  
    mov bx, num3  
    add bx, cx  
    sub  bx, 1     
    mov byte[bx],0
    sub cx, 1
    cmp cx, 0
    jz endset
    jmp set  

endset:
	mov byte[num1+15], 0
	mov byte[num2+15], 1

adding:  
    push    cx  
  
    mov     byte[cf], 0         ; 初始化进位为0  
    mov     cx, 16  
    next:  
        ; 计算an=an-1+an-2  
        mov     bx, num1  
        add     bx, cx  
        sub     bx, 1  
        mov     al, byte[bx]  
        mov     bx, num2  
        add     bx, cx  
        sub     bx, 1     
        add     al, byte[bx]      
        mov     bx, num3  
        add     bx, cx  
        sub     bx, 1     
        add     al, byte[cf]  
        mov     byte[bx], al  
        mov     byte[cf], 0     ; 进位加完后归0  
        cmp     byte[bx], 100   ; 每个单元存放的值最多为99,使用十进制的思想  
        jb      end  
        sub     byte[bx], 100  
        mov     byte[cf], 1     ; 若大于等于100,则进位 
        sub cx, 1
    	cmp cx, 0
    	jz endnext
    end:  
    	jmp next 

    antiadding:
        jmp adding

	endnext:
    	mov     cx, 16  
    copy:  
        mov     bx, num2  
        add     bx, cx  
        sub     bx, 1     
        mov     al, byte[bx]  
        mov     bx, num1  
        add     bx, cx  
        sub     bx, 1     
        mov     byte[bx], al    ; 将an-1赋值给an-2  
          
        mov     bx, num3  
        add     bx, cx  
        sub     bx, 1     
        mov     al, byte[bx]  
        mov     bx, num2  
        add     bx, cx  
        sub     bx, 1     
        mov     byte[bx], al    ; 将an赋值给an-1  
        sub cx, 1
    	cmp cx, 0
    	jz endcopy
   		jmp copy  
    endcopy:
    	pop cx  
		jmp antiadding  




finish:
	mov eax, 4
	mov ebx, 0
	mov ecx, NEWLINE
	mov edx, NEWLINE_LEN
	int 0x80;print new line

exit:
	mov eax, 1
	mov ebx, 0
	int 0x80 ;system exit                  