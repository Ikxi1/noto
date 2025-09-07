; functions


write:
	; arg 1 = rsi buffer
	; arg 2 = rdx buffer size
	push rax
	push rdi
	mov rax, 1
	mov rdi, 1
	syscall
	pop rdi
	pop rax
	ret


reset_file_ptr:
	; arg 1 = rdi file_ptr
	push rax
	push rsi
	push rdx
	mov rax, 8
	mov rsi, 0
	mov rdx, 0
	syscall
	pop rdx
	pop rsi
	pop rax
	ret

return:
	ret


; exit statements


exit_no_file:
	lea rsi, qword [no_file]
	mov rdx, no_file_len
	call write
	jmp exit

exit_bad_file:
	lea rsi, qword [bad_file]
	mov rdx, bad_file_len
	call write
	jmp exit

exit_read_fail:
	lea rsi, qword [read_fail]
	mov rdx, read_fail_len
	call write
	jmp exit

exit_empty_file:
	lea rsi, qword [empty_file]
	mov rdx, empty_file_len
	call write
	jmp exit

exit_size_fail:
	lea rsi, qword [size_file]
	mov rdx, size_file_len
	call write
	jmp exit

argc_fail:
	lea rsi, qword [argc_wrong]
	mov rdx, argc_wrong_len
	call write
	jmp exit

file_too_big:
	lea rsi, qword [file_big]
	mov rdx, file_big_len
	call write
	jmp exit

something_else:
	lea rsi, qword [undefined]
	mov rdx, undefined_len
	call write

exit:
; closefile:
    mov rax, 3
	mov rdi, qword [file_pointer]
    syscall
; moommap:
	mov rax, 11
	mov rdi, qword [file_buffer_pointer]
	mov rsi, qword [file_size]
	add rsi, 1 ; \0
	syscall
	mov rax, 11
	mov rdi, qword [pokke_buffer_pointer]
	shl rsi, 1
	add rsi, 1 ; \0
	syscall
quit:
	; leave
	mov rax, 60
	xor rdi, rdi
	syscall


endless_loop:
	jmp endless_loop


; data


section .bss
	file_size resq 1
	file_pointer resq 1
	delimiter_pointer1 resq 1
	delimiter_pointer2 resq 1
	pokke_buffer_pointer resq 1
	file_buffer_pointer resq 1
	save_pointer1 resq 1
	save_pointer2 resq 1
	string_position resq 1


section .data
	intermediate_file_buffer_size equ 1

	no_file db "Specify a file as the first argument.", 10
	no_file_len equ $ - no_file

	bad_file db "Couldn't open the file.", 10
	bad_file_len equ $ - bad_file

	read_fail db "Couldn't read the file.", 10
	read_fail_len equ $ - read_fail

	empty_file db "The specified file is empty.", 10
	empty_file_len equ $ - empty_file

	size_file db "Something went wrong getting the file size.", 10
	size_file_len equ $ - size_file

	undefined db "Undefined error", 10
	undefined_len equ $ - undefined

	argc_wrong db "Please specify an input file and delimiters.", 10
	argc_wrong_len equ $ - argc_wrong

	file_big db "Your file is way too big. Like >2^63-1 bytes.", 10
	file_big_len equ $ - file_big

	delimiters db 32,10,9,0
