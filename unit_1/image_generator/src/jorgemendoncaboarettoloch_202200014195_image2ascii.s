.intel_syntax noprefix

.section .text
.global main
main:
  // prologue
  push rbp
  mov rbp, rsp
  sub rsp, 48

  // getting argv[1]
  mov r12, [rsi + 8]
  // getting argv[2]
  mov r13, [rsi + 16]

  // input_file = fopen(input_path, "r")
  mov rdi, r12
  lea rsi, [rip + read_mode]
  call fopen@plt

  // writing the return value to the variable
  mov [rip + input_file], rax

  // output_file = fopen(output_path, "w")
  mov rdi, r13
  lea rsi, [rip + write_mode]
  call fopen@plt

  // writing the return value to the variable
  mov [rip + output_file], rax

  // fscanf(input_file, "%hhu", &pallete_size)
  mov rdi, [rip + input_file]
  lea rsi, [rip + get_byte_unisgned]
  lea rdx, [rip + pallete_size]
  call fscanf@plt

  // malloc(pallete_size)
  mov rdi, [rip + pallete_size]
  call malloc@plt
  // getting result of maloc and storing it
  mov [rip + pallete], rax
  
  // for (int i = 0; i < pallete_size; i++)
  read_pallete_init:
    mov rcx, 0
    mov rbx, [rip + pallete_size]
  read_pallete:
    cmp rcx, rbx
    je done_pallete
    
    // saving rcx in the stack, for security
    mov [rbp - 8], rcx

    // fscanf(input_file, "%hhx", &pallete[i])
    mov rdx, [rip + pallete]
    add rdx, rcx
    lea rsi, [rip + get_hexa_string]
    mov rdi, [rip + input_file]
    call fscanf@plt

    mov rcx, [rbp - 8]

    inc rcx
    jmp read_pallete

  done_pallete:
  // fscanf(input_file, "%u", &images)
  lea rdx, [rip + n_images]
  lea rsi, [rip + get_number_unsigned]
  mov rdi, [rip + input_file]
  call fscanf@plt

  read_images_init:
    mov rcx, 0  
    mov rbx, [rip + n_images]
  read_images:
    cmp rcx, rbx
    je done

    mov [rbp - 16], rcx
    
    // fscanf(input_file, "%u", &rows)
    lea rdx, [rip + rows]
    lea rsi, [rip + get_number_unsigned]
    mov rdi, [rip + input_file]
    call fscanf@plt

    // fscanf(input_file, "%u", &columns)
    lea rdx, [rip + columns]
    lea rsi, [rip + get_number_unsigned]
    mov rdi, [rip + input_file]
    call fscanf@plt
    
    mov rcx, [rbp - 16]
    // fprintf(output_file, "[%u]\n", i)
    mov rdx, rcx
    lea rsi, [rip + image_number_string]
    mov rdi, [rip + output_file]
    call fprintf@plt
    
    read_rows_init:
      mov rcx, 0
      mov r12, [rip + rows]
    read_rows:
      cmp rcx, r12
      je done_rows

      mov [rbp - 24], rcx
      read_columns_init:
        mov rcx, 0
        mov r13, [rip + columns]
      read_columns:
        cmp rcx, r13
        je done_columns

        mov [rbp - 32], rcx

        // fscanf(input_file, "%hhx", &pixel)
        lea rdx, [rip + pixel]
        lea rsi, [rip + get_hexa_string]
        mov rdi, [rip + input_file]
        call fscanf@plt

        // using r14 as left and r15 as right
        mov r14, [rip + pixel]  
        mov r15, r14
        // left = pixel >> 4
        shr r14, 4
        // right = pixel & 0x0F
        and r15, 0x0F

        // fprintf(output, "%c", pallete[left])
        mov rdx, [rip + pallete]
        add rdx, r14
        mov dl, [rdx]
        lea rsi, [rip + char_mask]
        mov rdi, [rip + output_file]
        call fprintf@plt

        // fprintf(output, "%c", pallete[right])
        mov rdx, [rip + pallete]
        add rdx, r15
        mov dl, [rdx]
        lea rsi, [rip + char_mask]
        mov rdi, [rip + output_file]
        call fprintf@plt

        mov rcx, [rbp - 32]
        inc rcx
        jmp read_columns
      done_columns:
      // fprintf(output, "\n")
      lea rsi, [rip + endline]
      mov rdi, [rip + output_file]
      call fprintf@plt

      mov rcx, [rbp - 24]
      inc rcx
      jmp read_rows
    done_rows:
    mov rcx, [rbp - 16]
    inc rcx
    jmp read_images
  done:
  // epilogue
  mov rdi, [rip + input_file]
  call fclose@plt

  mov rdi, [rip + output_file]
  call fclose@plt

  mov rdi, [rip + pallete]
  call free@plt

  mov rsp, rbp
  pop rbp

  ret

.section .data
  pallete: 
    .8byte 0
  input_file:
    .8byte 0
  output_file:
    .8byte 0
  rows: 
    .8byte 0
  columns: 
    .8byte 0
  n_images: 
    .8byte 0
  pallete_size: 
    .8byte 0
  pixel:
    .8byte 0

.section .rodata
  endline:
    .string "\n"
  get_hexa_string: 
    .string "%hhx"
  get_byte_unisgned:
    .string "%hhu"
  get_number_unsigned:
    .string "%u"
  write_mode:
    .string "w"
  read_mode:
    .string "r"
  image_number_string:
    .string "[%u]\n"
  char_mask:
    .string "%c"

