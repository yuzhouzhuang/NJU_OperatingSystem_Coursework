section .data
    msg: db '0S_Lab_1 Fibonacci', 0ah,'Please enter the numbers: ', 0ah
    msg_len equ $ - msg

    color_red: db 1Bh,'[31;1m',0
    .len equ $ - color_red
    color_blue: db 1Bh,'[34;1m',0
    .len equ $ - color_blue
    color_default: db 1Bh,'[37;0m',0
    .len equ $ - color_default

    NewLine: db 0Ah
    .len equ 1

    input_length: equ 44
    input_count: dd 0
    fib_count: dd 0

    fib_bit: dd 15
    TEN: dd 0Ah
    fib_result: times 15 db '0'
    e1: db 0Ah
    color_count: db 0

    bignumber1: times 100 db '0'
    bignumber_length equ 100
    bignumber2: times 99 db '0'
    e2: db '1'
    result_len : db 1

section .bss
    input: resb 44
    string_length: resb 1
    locate: resd 1
    num: resd 1
    input_numbers: resd 10

section .text
  global  _start:
  _start:
    mov eax, 4
    mov ebx, 1
    mov ecx, msg
    mov edx, msg_len
    int 80h 

    call InputNum
    call Fibonacci

  End:
    mov eax, 1 
    mov ebx, 0 
    int 80h 


InputNum:
    mov eax, 3
    mov ebx, 0
    mov ecx, input
    mov edx, input_length
    int 80h
    mov ebx, input
    mov ecx, ebx
    add ecx, eax 
    dec ecx
    mov byte[ecx], 20h 
    mov dword[locate], ebx
    InputNum_Loop:
      mov dh, byte[ebx]
      cmp dh, 20h
      je InputNum_NextChar
      inc ebx
      jmp InputNum_Loop
    InputNum_NextChar:
      mov edx, ebx
      mov ecx, [locate]
      sub dl, cl
      mov [string_length], dl
      push eax
      push ebx
      push ecx
      call StringConverter
      pop  ecx
      pop  ebx
      pop  eax
      inc ebx
      mov ecx, input
      mov edx, ebx
      sub edx, ecx
      cmp edx, eax
      je  InputNum_Finish
      mov dword[locate], ebx
      jmp InputNum_Loop
    InputNum_Finish:
      ret

  StringConverter:
    mov ecx, 0 
    mov ebx, 0 
    mov cl, [string_length] 
    CONVERT:
      cmp ch, cl
      je CONVERT_END
      mov edx, [locate]  
      movzx eax, ch
      add edx, eax
      mov ah, [edx]
      sub ah, 30h 
      movzx eax, ah

      cmp eax, 0
      je READ_NEXT 
      mov dh, cl
      sub dh, ch
      dec dh
    INC10:
      cmp dh, 0
      je READ_NEXT
      push edx
      mov edx, [TEN]
      mul edx
      pop edx
      dec dh
      jmp INC10
    READ_NEXT:
      add ebx, eax
      inc ch 
      jmp CONVERT
    CONVERT_END:
      mov ecx, [input_count]
      inc ecx
      mov [input_count], ecx
      dec ecx
      sal ecx, 2 
      mov edx, input_numbers
      add edx, ecx 
      mov [edx], ebx
      ret


  Fibonacci:
    mov eax, [input_count]
    ReadNumber:
      cmp eax, 0
      je Start
      mov ecx, input_numbers
      mov ebx, eax
      dec ebx
      sal ebx, 2
      add ebx, ecx
      mov ebx, [ebx]
      push ebx 
      dec eax
      jmp ReadNumber

    Start:
      mov eax, 0
      pop edx
      cmp eax, edx
      jne FibLoop
      mov edx, [fib_count]
      inc edx,
      mov [fib_count], edx
      pop edx
      call Output
     FibLoop:
       push eax
       mov eax, [fib_count]
       cmp eax,[input_count]
       je FibFinish
       pop eax  
       call FibAdd
       inc eax
       cmp eax, edx
       jne FibLoop
       mov edx, [fib_count]
       inc edx,
       mov [fib_count], edx
       pop edx 
       call Output
       jmp FibLoop
    FibFinish:
      pop eax
      push edx 
      ret

  FibAdd:
      push eax
      push ecx
      push edx
      xor ebx, ebx
      mov eax, bignumber1
      mov ecx, bignumber2
      add eax, bignumber_length
      add ecx, bignumber_length
      dec eax
      dec ecx
      mov dl, [result_len]
      mov dh, 0
      FibAddLoop:
        cmp dh, dl
        je  FibAddFinish  
        push edx
        
        mov dh, byte[ecx]
        mov dl, byte[eax]
        
        mov byte[eax], dh 

        add dh, dl
        add dh, bl 
        sub dh, 30h
        mov bl, 0
        cmp dh, 39h
        jle FibAddNext
        mov bl, 1
        sub dh, 10
        FibAddNext:
          mov byte[ecx], dh 
          pop edx
          inc dh
          dec ecx
          dec eax
          jmp FibAddLoop
      FibAddFinish:
        cmp bl, 0
        je FibAddEnd
        mov bh, [result_len]
        inc bh
        mov [result_len], bh
        mov byte[ecx], 31h
      FibAddEnd:
        mov bh, [result_len]
        pop edx
        pop ecx
        pop eax
        ret

  Output:
    push eax
    push ebx
    push ecx
    push edx
    mov eax, 4
    mov ebx, 1
    mov bl, [color_count]
    cmp bl, 0
    je BLUE
    jmp RED
    BLUE:
      mov bl, 1
      mov [color_count], bl
      mov ecx, color_blue
      mov edx, color_blue.len
      int 80h
      jmp Result
    RED:
      mov bl, 0
      mov [color_count], bl
      mov ecx, color_red
      mov edx, color_red.len
      int 80h
    Result:
      mov ecx, bignumber2
      mov dh, [result_len]
      mov dl, bignumber_length
      movzx eax, dl
      movzx edx, dh
      add ecx, eax
      sub ecx, edx
      mov eax, 4
      mov ebx, 1
      int 80h
      mov eax, 4
      mov ebx, 1
      mov ecx, NewLine
      mov edx, NewLine.len
      int 80h
      pop  edx
      pop  ecx
      pop  ebx
      pop  eax
      ret