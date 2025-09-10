%include "common.inc"
section .text
global get_pokke_buffer
global moommap
extern strtok_r
extern strtoktok_r

; arg 1  = rdi const char *file name
; arg 2  = rsi const char *delimiters
; arg 3  = rdx const char *persistent delimiters
; return = rax char *token buffer

get_pokke_buffer:
    enter 0, 0

    mov [delimiter_pointer1], rsi
    mov [delimiter_pointer2], rdx
    ; when called from the command line, maybe to pipe into sth else
    ; cmp rdi, 4
    ; jne argc_fail
    ; mov rax, qword [rsi + 16]
    ; mov [delimiter_pointer1], qword rax
    ; mov rax, qword [rsi + 24]
    ; mov [delimiter_pointer2], qword rax
open_file:
    mov rax, 2
    ; file_name already in rdi
    test rdi, rdi
    js exit_no_file
    cmp rdi, 0
    jz exit_no_file
    mov rsi, 0
    mov rdx, 0
    syscall
    test rax, rax
    js exit_bad_file
    mov qword [file_pointer], rax
get_file_size:
    ; lseek syscall == fseek in c
    mov rax, 8
    mov rdi, qword [file_pointer]
    mov rsi, 0
    mov rdx, 2
    syscall
    test rax, rax
    js exit_size_fail
    mov rsi, rax
    clc
    shl rax, 1
    jc file_too_big
    mov [file_size], qword rsi
    call reset_file_ptr
allocate_file_buffer:
    mov rax, 9
    xor rdi, rdi
    add rsi, 1
    mov rdx, 3
    mov r10, 0x22
    xor r9, r9
    xor r8, r8
    syscall
    test rax, rax
    js something_else
    mov qword [file_buffer_pointer], rax
copy_file:
    xor rax, rax
    mov rdi, qword [file_pointer]
    mov rdx, rsi
    mov rsi, qword [file_buffer_pointer]
    syscall
    test rax, rax
    js exit_read_fail
    call reset_file_ptr
close_file:
    mov rax, 3
    syscall
    test rax, rax
    js something_else
allocate_pokke_buffer:
    ; multiply file_size x2 for worst case scenario
    shl rdx, 1
    sub rdx, 1 ; i already added one extra byte for EOF, now it's doubled
    mov rsi, rdx
    mov rax, 9
    xor rdi, rdi
    mov rdx, 3
    mov r10, 0x22
    xor r9, r9
    xor r8, r8
    syscall
    test rax, rax
    js something_else
    mov [pokke_buffer_pointer], qword rax

pokkenize:
    mov rdi, qword [file_buffer_pointer]
    mov rsi, qword [delimiter_pointer1]
    mov rdx, save_pointer1
    call strtok_r
    call get_token_tokens
pokkenize_loop:
    mov rdi, 0
    mov rsi, qword [delimiter_pointer1]
    mov rdx, save_pointer1
    call strtok_r
    cmp rax, 0
    je end                 ; write_tokens or end, based on if you call it in another program or want to pipe the output
    call get_token_tokens
    jmp pokkenize_loop

    ; when used inside another program
end:
    mov rax, 11
    mov rdi, qword [file_buffer_pointer]
    mov rsi, qword [file_size]
    add rsi, 1 ; \0
    syscall
    mov rax, [pokke_buffer_pointer]
    leave
    ret

    ; when used standalone, but why
; write_tokens:
;     mov rsi, qword [pokke_buffer_pointer]
;     mov rdx, qword [file_size]
;     shl rdx, 1
;     inc rdx
;     call write

;     jmp exit

get_token_tokens:
    mov rdi, rax
    mov rsi, qword [delimiter_pointer2]
    mov rcx, qword [pokke_buffer_pointer]
    mov r8, string_position
    call strtoktok_r
    cmp rax, 0
    jne get_token_tokens_loop
    ret
get_token_tokens_loop:
    mov rsi, qword [delimiter_pointer2]
    mov rcx, qword [pokke_buffer_pointer]
    mov r8, string_position
    call strtoktok_r
    cmp rax, 0
    jne get_token_tokens_loop
    ret


moommap:
    enter 0, 0
    push rax
    push rdi
    push rsi

    mov rax, 11
    mov rdi, qword [pokke_buffer_pointer]
    mov rsi, [file_size]
    shl rsi, 1
    add rsi, 1 ; \0
    syscall

    pop rsi
    pop rdi
    pop rax
    leave
    ret
