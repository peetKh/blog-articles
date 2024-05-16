[SECTION .text]
global _start
_start:

jmp payload
return_pad:
pop EAX
; <SHELLCODE_INSTRUCTIONS>
nop
nop
nop

payload:
call return_pad
; <DATA_BUFFER>
