%define last_word 0

%macro colon 2
%%previous_word: dq last_word ;
db %1, 0
%2:
%define last_word %%previous_word
%endmacro


; colon "Low-Level Programming", Low-Level-Programming
; db "A very interesting book", 0
; colon "rubber-duck-typing", rubber-duck-typing 
; db "A very informative blog", 0