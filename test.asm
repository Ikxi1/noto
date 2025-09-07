section .text
global _start

_start:
    mov rdi, [rsp + 16]

    xor rax, rax
    mov rcx, -1
    cld ; set DF to 0 == UP -> increments rdi
    repne scasb
    not rcx
    mov rax, rcx

    mov rax, 60
    xor rdi, rdi
    syscall
