section .data
msg: db 'Please enter the numbers: ', 0ah
msg_len equ $ - msg
space: db '',20h
space_len equ $ - space
newline: db '',0ah
nl_len equ $ - newline
color_red: db 1Bh,'[31;1m',0
.len equ $ - color_red
color_blue: db 1Bh,'[34;1m',0
.len equ $ - color_blue
color_default: db 1Bh,'[37;0m',0
.len equ $ - color_default

section .bss
input_buffer: resb 1 ;输入缓存
input_len: resb 1 ;输入数量
num_buffer: resb 1 ;输入(总)数字缓存
array: resb 9 ;存储所有输入的数组
max: resb 1 ;最大值(最后一次输入值)
cur_op: resb 1 ;当前需要输出的斐波那契项的项数
index: resb 1 ;当前需要输出的斐波那契是第几个输出
current: resb 1 ;存储当前遍历到的斐波那契项
first: resb 1 ;当前较小斐波那契项
second: resb 1 ;当前较大斐波那契项
count: resb 1 ;记录当前遍历到的斐波那契项的项数
op_ct: resb 1
op_tmp: resb 1
op_nm: resb 1
clr_ptr: resb 1

section .text
GLOBAL main:
main:
	mov eax, 4
	mov ebx, 1
	mov ecx, msg
	mov edx, msg_len
	int 80h ;显示输入提示信息

	mov al, 0
	mov byte[input_len], al ;初始化输入数量为0
	mov byte[num_buffer], al ;初始化输入数字为0

input:
	mov eax, 3
	mov ebx, 0
	mov ecx, input_buffer
	mov edx, 1 ;一次读入1个字节
	int 80h

	cmp byte[input_buffer], 20h
	je store ;若为空格，则存储当前数字

	cmp byte[input_buffer], 0ah
	je store ;若为换行符(输入结束)，存储最后一个数字
	mov al, byte[num_buffer]
	mov bl, 10
	mul bl
	mov bl, byte[input_buffer]
	sub bl, 30h ;去除输入格式
	add al, bl
	mov byte[num_buffer], al
	jmp input

store:
	xor esi, esi
	xor edx, edx
	add dl, byte[input_len] ;设置esi为当前数字序号
	mov esi, edx
	mov al, byte[num_buffer]
	mov byte[array + esi], al ;存回数组
	mov al, byte[input_len]
	add al, 1
	mov byte[input_len], al ;数字数加1
	mov al, 0
	mov byte[num_buffer], al ;数值清零
	cmp byte[input_buffer], 0ah
	jne input
	
	mov al, byte[input_len]
	sub al, 1
	mov byte[input_len], al ;获取有效最大偏移
	
	xor esi, esi
	mov dl, byte[input_len] ;设置esi为当前数字序号
	mov dh, 0
	mov si, dx
	mov al, byte[array + esi]
	mov byte[max], al ;设置最大所需计算的斐波那契项
	mov al, byte[array]
	mov byte[cur_op], al ;设置当前所需输出的项为第一项
	mov byte[index], 1
	
fibonacci:
	mov byte[first], 0 ;设置第0项为0
	mov byte[second], 1 ;设置第1项为1
	mov byte[current], 1 ;设置当前等待输出项为第一项
	mov byte[count], 1 ;设置等待输出项的项数
	mov byte[clr_ptr], 0
	
loop:
	mov al, byte[cur_op]
	cmp byte[count], al
	mov al, byte[current]
	mov byte[op_ct], al
	mov al, 0
	mov byte[op_nm], al
	je output_c
	mov al, byte[second]
	add al, byte[first]
	mov byte[second], al
	mov al, byte[current]
	mov byte[first], al
	mov al, byte[second]
	mov byte[current], al
	mov al, byte[count]
	add al, 1
	mov byte[count], al
	jmp loop

output_c:
	cmp byte[clr_ptr], 0
	je cred
	
	mov al, 0
	mov byte[clr_ptr], al
	mov eax, 4
	mov ebx, 1
	mov ecx, color_blue
	mov edx, color_blue.len
	int 80h
	jmp output
	
cred:
	mov al, 1
	mov byte[clr_ptr], al
	mov eax, 4
	mov ebx, 1
	mov ecx, color_red
	mov edx, color_red.len
	int 80h

output:
	xor eax, eax
	mov bl, 10
	mov cl, byte[op_ct]
	cmp cl, 0
	je op_finish
	add al, byte[op_ct]
	div bl
	mov byte[op_ct], al
	add ah, 30h
	mov byte[op_tmp], ah
	xor ebx, ebx
	add bl, byte[op_tmp]
	push bx
	mov al, byte[op_nm]
	add al, 1
	mov byte[op_nm], al
	jmp output
	
op_finish:
	mov al, byte[op_nm]
	cmp al, 0
	je op_reserve
	sub al, 1
	mov byte[op_nm], al
	xor ebx, ebx
	pop bx
	mov byte[op_tmp], bl
	mov eax, 4
	mov ebx, 1
	mov ecx, op_tmp
	mov edx, 1
	int 80h
	jmp op_finish
	
op_reserve:	
	mov al, byte[count]
	cmp byte[max], al
	mov eax, 4
	mov ebx, 1
	mov ecx, space
	mov edx, space_len
	int 80h
	je exit
	xor esi, esi
	mov dl, byte[index] ;设置esi为当前数字序号
	mov dh, 0
	mov si, dx
	mov al, byte[array + esi]
	mov byte[cur_op], al
	add dl, 1
	mov byte[index], dl
	jmp loop

exit:
	mov eax, 4
	mov ebx, 1
	mov ecx, newline
	mov edx, nl_len
	int 80h
	mov eax, 4
	mov ebx, 1
	mov ecx, color_default
	mov edx, color_default.len
	int 80h
	mov eax, 1
	mov ebx, 0
	int 80h ;程序结束