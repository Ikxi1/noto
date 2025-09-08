global strlen_asm

strlen_asm:
	; arg 1 = rdi string_ptr
	; rcx is overwritten
	; return = rax strlen

	xor rax, rax
	mov rcx, -1
	cld
	repne scasb
	not rcx
	sub rdi, rcx
	mov rax, rcx

    ret
