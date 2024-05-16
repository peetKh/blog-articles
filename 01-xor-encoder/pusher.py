buff = b'cmd /c echo blah\x00'
buff += b'\x00' * (-len(buff) % 4)

# Breakpoint
sc =  b'\xCC' 
# Push string 
sc += b''.join( (b'\x68' + buff[i:i+4]) for i in range(len(buff)-4, -1, -4) )
# Mov EAX, ESP
sc += b'\x89\xE0'

with open('shellcode-push.bin','wb') as f:
    f.write(sc)
