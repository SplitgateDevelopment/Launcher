PUBLIC _NtUserGetAsyncKeyState

.code

_NtUserGetAsyncKeyState PROC

	mov r10, rcx
	mov eax, 3735928559
	syscall
	ret

_NtUserGetAsyncKeyState ENDP

END