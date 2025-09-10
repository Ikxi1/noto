; arg 1 = rdi char *string
; arg 2 = rsi char *delimiters
; arg 3 = rcx char *token_buffer
; arg 4 = r8 size_t *string_position
; return = rax 0 == end of string; rax != 0 string continues

section .text

global strtoktok_r

extern strspn
extern strcspn
extern memcpy

strtoktok_r:
    push rbp
    mov rbp, rsp
    sub rsp, 16

.l1:
    cmp byte [rdi], 0
    jne .l2
    mov rax, 0
    jmp .ret
.l2:
    mov [.string], qword rdi
    mov [.delimiters], qword rsi
    mov [.token_buffer], qword rcx
    mov [.string_position], qword r8
    call strspn ; rdi string rsi delimiters
    cmp rax, 0
    je .l4
    mov r9, qword [.string_position]
    mov r9, qword [r9]
.l3:
    mov al, byte [rdi]
    mov rcx, qword [.token_buffer]
    mov [rcx + r9], al
    inc r9
    mov byte [rcx + r9], 0
    inc r9
    inc rdi
    mov rsi, qword [.delimiters]
    call strspn
    cmp rax, 0
    jne .l3
    mov r8, qword [.string_position]
    mov [r8], qword r9
    mov [.string], qword rdi
.l4:
    cmp byte [rdi], 0
    jne .l5
    mov rax, 0
    jmp .ret
.l5:
    mov rsi, qword [.delimiters]
    call strcspn
    add rax, 1 ; for the following newline
    mov rdi, qword [.token_buffer]
    mov r8, qword [.string_position]
    add rdi, qword [r8]
    mov rsi, qword [.string]
    mov rdx, rax
    call memcpy
.l6:
    add [.string], qword rdx
    mov r8, qword [.string_position]
    add [r8], qword rdx
    mov rdi, qword [.token_buffer]
    add rdi, qword [r8]
    mov byte [rdi], 0
    inc qword [r8]
    mov rdi, qword [.string]
    cmp byte [rdi], 0
    jne .l7
    mov rax, 0
    jmp .ret
.l7:
    mov rsi, qword [.delimiters]
    call strspn
    cmp rax, 0
    je .l9
    mov r9, qword [.string_position]
    mov r9, qword [r9]
.l8:
    mov al, byte [rdi]
    mov rcx, qword [.token_buffer]
    mov byte [rcx + r9], al
    inc r9
    mov byte [rcx + r9], 0
    inc r9
    inc rdi
    mov rsi, qword [.delimiters]
    call strspn
    cmp rax, 0
    jne .l8
    mov r8, [.string_position]
    mov [r8], qword r9
    mov [.string], qword rdi
.l9:
    cmp byte [rdi], 0
    jne .l10
    mov rax, 0
    jmp .ret
.l10:
    mov rax, rdi
.ret:
    leave
    ret


section .bss
    .string_position resq 1
    .string resq 1
    .delimiters resq 1
    .token_buffer resq 1
