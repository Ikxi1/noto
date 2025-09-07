global strlen_asm

strlen_asm:
	; arg 1 = rdi string_ptr
	; rcx is overwritten
	; return = rax strlen

	xor rax, rax
    xor rcx, rcx
.l1:
	inc rcx
	scasb
	jnz .l1
	sub rdi, rcx
	mov rax, rcx

    ret
