global find_word
extern string_equals

; *** Arguments ***
; rdi - a pointer to a null terminated key string
; rsi - a pointer to the last word in the dictionary
; returns: a pointer to the word, else 0
find_word:
	xor rax, rax
.loop:
	test rsi, rsi 			; rsi is null means there are no words left in the dictionary
	jz .end
	push rdi
	push rsi
	add rsi, 8				; because first 8 bytes are occupied with a pointer to the previous word
	call string_equals		; check if key is equal to the string we are looking for
	pop rsi					; pop rsi, so it is -8 again
	pop rdi
	test rax, rax			; if string_equals returned 1 then the word is found
	jnz .found
	mov rsi, [rsi]			; move to the previous word in dictionary (following the pointer in the entry)
	jmp .loop
.found:
	mov rax, rsi			; return a pointer to the found word
.end:
	ret


		
	
	