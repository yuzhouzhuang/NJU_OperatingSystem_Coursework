section .text
  global  _start:
  _start:
    mov eax, 4 ;系统调用号 Write
    mov ebx, 1 ;标准输出设备
    mov ecx, Hello ;指向输出字符串
    mov edx, hello_length ;字符数
    int 80h
    call GetInput

    call Fib
    ;call PT ; OK
    ;call PT_NUM
    mov eax, 4 
    mov ebx, 1 
    mov ecx, NewLine 
    mov edx, NewLine.len
    int 80h
  End:
    ; 结束
    mov eax, 1 ;系統調用號碼
    mov ebx, 0 ;參數
    int 80h ; 触发操作系统中断

  Fib:
    mov eax, [input_count]
    Fib_Break_Point:
      ; 输入序列压栈
      cmp eax, 0
      je Fib_Start_Cal
      mov ecx, input_numbers
      mov ebx, eax
      dec ebx
      sal ebx, 2
      add ebx, ecx

      mov ebx, [ebx]
      push ebx ; in stack
      dec eax
      jmp Fib_Break_Point

    Fib_Start_Cal:
      mov eax, 0 ;计数器
      mov ebx, 0
      mov ecx, 1 ;结果寄存器
      pop edx ; 断点寄存器

      cmp eax, edx
      jne LOOP_ADD
      mov [num], ecx
      ;Fib 命中
      mov edx, [fib_counter]
      inc edx,
      mov [fib_counter], edx
      pop edx

      call PT_NUM
      ;jmp FINISH_FIB
     LOOP_ADD:
       push eax
       ;push ebx
       mov eax, [fib_counter]
       cmp eax,[input_count]
       je FINISH_FIB
       
       pop eax  
       push edx
       mov edx, ecx ;存储fx-1
       add ecx, ebx ;fx-1 + fx-2  
       mov ebx, edx ; Cal
       
       pop edx
       inc eax

       cmp eax, edx
       jne LOOP_ADD
       mov [num], ecx
       ;Fib 命中
       mov edx, [fib_counter]
       inc edx,
       mov [fib_counter], edx
       pop edx 
       ;mov [num], ecx
       call PT_NUM
       jmp LOOP_ADD
    FINISH_FIB:
      pop eax
      push edx ; Return Address
      ret
  
  PT_NUM:
    push eax
    push ebx
    push ecx
    push edx
    call Int2Str
    call PT_COLOR
    pop  edx
    pop  ecx
    pop  ebx
    pop  eax
    ret

  PT_COLOR:
    mov eax, 4
    mov ebx, 1
    mov bl, [fib_mode]
    cmp bl, 0
    je MODE_1
    jmp MODE_2
    MODE_1:
    mov bl, 1
    mov [fib_mode], bl
    mov ecx, color_blue
    mov edx, color_blue.len
    int 80h
    jmp PRINT_OUT
    MODE_2:
    mov bl, 0
    mov [fib_mode], bl
    mov ecx, color_red
    mov edx, color_red.len
    int 80h
    PRINT_OUT:
    mov ecx, fib_result
    mov edx, [fib_ac_bit]
    mov eax, [fib_bit]
    add ecx, eax
    sub ecx, edx
    mov eax, 4
    mov ebx, 1
    int 80h
    mov eax, 4
    mov ebx, 1
    mov ecx, Seperator
    mov edx, Seperator.len
    int 80h
    ret

  P:
    mov ecx, Hello ;指向输出字符串
    mov edx, hello_length ;字符数
    mov eax, 4 ;系统调用号 Write
    mov ebx, 1 ;标准输出设备
    int 80h
    ret

  Int2Str:
    mov eax, 0
    mov [fib_ac_bit], eax
    mov eax, DWORD[num]
    mov ecx, [fib_bit]
    add ecx, fib_result
    dec ecx
    mov ebx, 10
    LOOP_STR:
      cmp eax, 10
      jl FINISH_STR_TRANS
      mov edx, 0
      div ebx
      add dl, 30h ;转为字符
      mov byte[ecx], dl ;存储
      dec ecx
      call BIT_INC
      jmp LOOP_STR
    FINISH_STR_TRANS:
      add al,30h
      mov byte[ecx], al ;存储
      inc ecx
      call BIT_INC
      ret
   
  BIT_INC:
    mov edx, [fib_ac_bit]
    inc edx
    mov [fib_ac_bit], edx
    ret 
  Te:
    ;mov eax, test_data
    ;add eax, 4
    ;mov ebx, [eax]
    ;pop eax
    ;mov bl, [a_length]
    ;cmp ebx, [a_length]
    ;je End
    ;call P
    ;jmp End
    ret

  GetInput:
    mov eax, 3
    mov ebx, 0
    mov ecx, Input
    mov edx, input_length
    int 80h

    ; eax 存有现有字符长度
    ; 对输入末端处理
    mov ebx, Input
    mov ecx, ebx
    add ecx, eax
    dec ecx
    mov byte[ecx], 20h ;改换行为空格
    ; 首字符位置
    mov dword[number_locate], ebx
    ;inc eax
    ; 获取输入数组
    GetInput_Loop:
      mov dh, byte[ebx]
      cmp dh, 20h
      je GetInput_NextChar
      inc ebx
      jmp GetInput_Loop
    GetInput_NextChar:
      mov edx, ebx
      mov ecx, [number_locate]
      sub edx, ecx
      ; edx = 数字字符长度
      mov [a_length], dl

      ; 
      push eax
      push ebx
      push ecx
      call Str2Int
      pop  ecx
      pop  ebx
      pop  eax
      ;jmp End
      ;
      ;判断是否到结尾。如果到结尾则ret
      inc ebx ; 下一数字字符位置
      mov ecx, Input
      mov edx, ebx
      sub edx, ecx
      cmp edx, eax
      je  GetInput_Finish
      mov dword[number_locate], ebx
      jmp GetInput_Loop
    GetInput_Finish:
      ;jmp End
      ret
    

    ;mov [a_length], eax
    ;call Str2Int
    ;ret

  ;字符串转数字
  Str2Int:
    mov ch, 0 ; 计数器, 高位低索引
    mov ebx, 0 ;结果寄存器
    mov cl, [a_length] ;输入长度  
    ;//TODO:
    LOOP_CHAR:
      cmp ch, cl
      je FINISH_INT_TRANS
      ;获取字符
      mov edx, [number_locate]  
      movzx eax, ch
      add edx, eax
      mov ah, [edx]
      sub ah, 30h ; dec the '0'
      movzx eax, ah

      cmp eax, 0
      je NEXT_LOOP ;为0无需计算
      ; 不为0,计算累加结果
      mov dh, cl
      sub dh, ch
      dec dh;计算需要乘以多少个10

      MUL_TEN:
        cmp dh, 0
        je NEXT_LOOP
        push edx
        mov edx, [TEN_D]
        mul edx
        pop edx
        dec dh
        jmp MUL_TEN
    NEXT_LOOP:
      add ebx, eax
      inc ch ;下一个字符
      jmp LOOP_CHAR
    FINISH_INT_TRANS:
      mov ecx, [input_count]
      ; 计数递增
      inc ecx
      mov [input_count], ecx
      ; 地址偏移
      dec ecx
      sal ecx, 2 ;左移2位
      mov edx, input_numbers ;
      add edx, ecx ; base + offset
      mov [edx], ebx  
      ret

section .data
    color_red:      db  1Bh, '[31;1m', 0
    .len            equ $ - color_red
    color_blue:     db  1Bh, '[34;1m', 0
    .len            equ $ - color_blue
    color_default:  db  1Bh, '[37;0m', 0
    .len            equ $ - color_default
    Seperator: db '  '
    .len equ 2
    NewLine: db 0Ah
    .len equ 1
    Hello: db 'Hello. Please Input Fib Number', 0Ah
    hello_length: equ 31
    input_length: equ 30 ; 允许输入30个字符。输入格式为 num（空格）num（空格）num \n
    input_count: dd 0
    fib_counter: dd 0
    fib_ac_bit: dd 0
    fib_bit: dd 15
    TEN: dw 0Ah
    TEN_D: dd 0Ah
    fib_result: times 15 db '0' , 0Ah
    fib_mode: db 0
section .bss
    Input: resb 30
    a_length: resb 1
    number_locate: resd 1
    num: resd 1
    input_numbers: resd 10