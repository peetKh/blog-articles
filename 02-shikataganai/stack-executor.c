#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#define CHUNK_SIZE 128

int loadFile(char **oBuffer, size_t *oDataSize, char *filename){
  // Load binary data from file

  *oBuffer = NULL;
  *oDataSize = 0;

  FILE * fs;
  char *buffer = NULL;
  size_t bytesRead = 0;
  size_t bufferSize = 0;
  size_t dataSize = 0;

  fs = fopen(filename, "rb");
  if ( fs == NULL ){
    printf("ERR: Could not open %s.\n", filename);
    return 1;
  }

  while (1){
    if ( dataSize + CHUNK_SIZE > bufferSize ){
      bufferSize = dataSize + 2*CHUNK_SIZE;
      buffer = realloc(buffer, bufferSize);
      if ( buffer == NULL ){
        printf("ERR: Could not allocate data buffer.\n");
        free(buffer);
        return 1;
      }
    }
    bytesRead = fread( buffer + dataSize, 1, CHUNK_SIZE,  fs);
    dataSize += bytesRead;
    if ( bytesRead == 0 )
      break;
  }

  if ( ferror(fs) ){
    printf("ERR: Error while reading file.\n");
    free(buffer);
    return 1;
  }

  printf("Loaded buffer: %d bytes at %p\n", dataSize, buffer);
  *oBuffer = buffer;
  *oDataSize = dataSize;
  return 0;
}

void * getRetAddr(){
   __asm__(
    "mov (%ebp), %eax\n"  // Stack frame base of caller function
    "add $4, %eax \n"     // Return address of the caller function
    );  
}

int smashTheStack(char *shellcode, size_t shellcodeSize){


  // Get address of return address
  void * retAddr = getRetAddr();

  // Overwrite return address
  *((char **) retAddr) = retAddr + 4;

  // Copy shellcode on stack
  memcpy( retAddr + 4, shellcode, shellcodeSize);

  __asm__("int3 \n"); // Have a break

  // Kaboom
  return 0;

}

int main(int argc, char *argv[]) {
  
  if (argc < 2) {
    printf("Usage: %s <binary-shellcode-file>\n", argv[0]);
    return 1;
  }

  char *shellcode;
  size_t shellcodeSize;

  if ( loadFile( &shellcode, &shellcodeSize, argv[1]) ){
    printf("Could not read shellcode. Quitting.\n");
    return 1;
  }

  smashTheStack( shellcode, shellcodeSize);

}
