global _start
extern find_word
extern string_length
extern read_word
extern print_string
extern print_newline
extern print_error
extern exit

section .rodata
msg_notfound : db "There is no such word :(", 0
msg_toolongword : db "The word is too long!", 0


section .text
%include "colon.inc"
%include "words.inc"

_start:
    sub rsp, 256                ; allocate 256 bytes on the stack for input
    mov rsi, 255                ; in read_word: rsi - buffer size ; 1 character is null terminator
    mov rdi, rsp                ; in read_word: rdi - buffer address 
    call read_word
    .debug:
    test rax, rax               ; if 0 then word is too long for buffer
    jz .too_long_word
    mov rdi, rsp                ; in find_word: rdi - a pointer to string
    mov rsi, last_word          ; in find_word: rsi - a pointer to the last entry
    call find_word
    test rax, rax               ; 0 if not found
    jz .not_found
    lea rdi, [rax + 8]          ; to print the value of the word, we need to know it's offset
                                ; from the "previous_word" label; previous_word pointer itself
                                ; takes 8 bytes, now we need to find out the length of the key
                                ; and then add 1 for a null terminator
    call string_length
    lea rdi, [rdi + rax + 1]    ; pointer to key + length of the key + null terminator
    call print_string
    call print_newline
    call exit
.too_long_word:
    mov rdi, msg_toolongword
    call print_error
    call exit
.not_found:
    mov rdi, msg_notfound
    call print_error
    call exit
