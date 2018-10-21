section .text

;arguments: rdi, rsi, rdx, rcx, r8, r9
;callee-saved: rbx, rbp, rsp, r12-r15
;left-free r10-r11 ||maybe ^r12-r15^
string_length:
    xor rax, rax
    .loop:
        cmp byte[rdi + rax], 0
        jz .string_length_ret
        inc rax
        jmp .loop
    .string_length_ret:
    ret

print_string:
    call string_length
    mov rsi, rdi
    mov rdx, rax
    mov rax, 1
    mov rdi, 1
    syscall
    ret


print_char:
push rdi
mov rdi, rsp
call print_string
pop rdi
ret

print_newline:
    mov rdi, 0xA
    call print_char
    ret


print_uint:
    push 0
    mov rsi, rsp
    times 8 push 0                  ; creating buffer on stack
    mov rax, rdi                    ; devidend
    mov rdx, 0                      ; end of the future string
    mov [rsi], rdx
    dec rsi
   .loop:
    mov rbx, 10                     ; devider
    mov rdx, 0                      ; remainder
    div rbx                         ; rax div rbx
    add rdx, 0x30                   ; add 0x30 so that number matches it's code in ASCII
    mov [rsi], dl                   ; move to buffer
    dec rsi
    cmp rax, 0                      ; check if number has ended
    jnz .loop
    inc rsi
    mov rdi, rsi                    ; pass string address on stack as argument to print_string 
    call print_string       
    times 9 pop rax                 ; restore rsp
    ret


print_int: ;wtf
    mov rax, rdi
    cmp rax, 0
    jl .neg
    call print_uint
    jmp .end
    .neg:
    mov rdi, '-'
    push rax
    call print_char
    pop rax
    neg rax
    mov rdi, rax
    call print_uint
    .end:
    ret

; parameters: rdi, rsi - pointers to strings
; returns rax: 1 if string are equal, 0 if not
string_equals:
    xor rcx, rcx
    .string_equals_loop:
    mov r10b, [rdi+rcx] ; maybe byte
    mov r11b, [rsi+rcx]
    cmp r10b, r11b
    jne .string_equals_fail
    inc rcx
    test r10b, r10b
    jz .string_equals_null_check
    jmp .string_equals_loop
    .string_equals_null_check:
    test r11b, r11b
    jnz .string_equals_fail
    mov rax, 1
    ret
    .string_equals_fail:
    mov rax, 0
    ret

; uses: rax, rdi, rdx, rsi
read_char:
    xor rax, rax ; syscall #1 read
    xor rdi, rdi; stdin $0
    mov rdx, 1 ; read 1 byte
    push rax ; reserve place on stack for input
    mov rsi, rsp ; save char on stack
    syscall
    pop rax ; return char in rax
    ret 

; arguments: rdi - buffer address, rsi - buffer size
; uses: rcx
; returns rax: buffer address; rdx: word length
read_word:
    push r12                        ; save used registers
    push r13
    mov r12, rdi                    ; move buffer address and buffer size to callee-saved regs
    mov r13, rsi                    ; to avoid pushing and popping them later
    .read_word_skip_whitespaces:
    call read_char
    cmp al, 0x9                     ; tabulation
    je .read_word_skip_whitespaces
    cmp al, 0x10                    ; line break
    je .read_word_skip_whitespaces
    cmp al, 0x20                    ; space
    je .read_word_skip_whitespaces
    mov rcx, -1                     ; initialize rcx with -1
    .read_word_reading_chars:
    inc rcx                         ; increase the letter counter
    cmp al, 0                       ; check for null-termination
    je .read_word_success
    cmp al, 0x20                    ; check for space after word
    je .read_word_success
    mov [r12+rcx], al               ; move the result of read_char to buffer
    cmp r13, rcx                    ; check if letter count is bigger than buffer size
    jbe .read_word_fail
    push rcx
    call read_char                  ; read next char
    pop rcx
    jmp .read_word_reading_chars
    .read_word_fail:
    mov rax, 0
    pop r13
    pop r12
    mov rdx, rcx
    ret
    .read_word_success:
    mov byte[r12+rcx], 0
    mov rax, r12                    ; return buffer address or 0
    pop r13                         ; restore used registers
    pop r12
    mov rdx, rcx
    ret

; argmunets: rdi - pointer to string
; returns rax: number, rdx : length
parse_uint:
    xor rsi, rsi
    xor rcx, rcx
    xor rax, rax
    mov r10, 10
    .parse_uint_loop:
    mov sil, [rdi+rcx]
    cmp sil, 0x30
    jb .parse_uint_ret
    cmp sil, 0x39
    ja .parse_uint_ret
    sub sil, 0x30
    mul r10
    add rax, rsi
    inc rcx
    jmp .parse_uint_loop
    .parse_uint_ret:

    mov rdx, rcx
    ret 

; arguments: rdi - pointer to string
; returns rax: number; rdx: length
parse_int:
    cmp byte[rdi], '-'
    je .parse_int_negative
    call parse_uint
    ret
    .parse_int_negative:
    inc rdi
    call parse_uint
    test rdx, rdx
    jz .parse_int_ret
    neg rax
    inc rdx
    .parse_int_ret:
    ret 

; parameters: rdi - pointer to string, 
;             rsi - pointer to buffer,
;             rdx - buffer length
; returns pointer to buffer or 0 if failed
string_copy:
    xor r10, r10
    push rsi
    push rdi
    push rdx
    call string_length
    pop rdx
    pop rdi
    pop rsi
    inc rax
    cmp rdx, rax
    jl .string_copy_fail
    xor rcx, rcx
    .loop:
    mov r10, [rdi+rcx]
    mov [rsi+rcx], r10
    inc rcx
    dec rax
    jnz .loop
    jmp .string_copy_end
    
    .string_copy_fail:
    mov rax, 0
    .string_copy_end:
    ret