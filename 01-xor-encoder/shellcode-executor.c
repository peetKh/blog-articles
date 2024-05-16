// ---- shellcode-executor.c ----
/*
  Execute a shellcode from binary file
  A buffer with execute permission is alocated and filled with the shellcode
*/


#include <windows.h>
#include <memoryapi.h>
#include <stdio.h>
#include <fileapi.h>

#define CHUNK_SIZE 1024 

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage: %s <shellcode-file>\n", argv[0]);
        exit(0);
    }

    // Open the binary file

    HANDLE hFile;
    LPCSTR filename = argv[1];
    hFile = CreateFileA(filename, 
        GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("ERROR %d. Could not open file %s.\n"
            , GetLastError(), filename);
        CloseHandle(hFile);
        exit(1);
    }

    // Read whole payload

    DWORD payloadSize = 0;
    DWORD bytesRead;
    char *payload = NULL;
    char chunk[CHUNK_SIZE];
    while (ReadFile(hFile, chunk, CHUNK_SIZE, &bytesRead, NULL)
            && bytesRead > 0) {
        payload = realloc(payload, payloadSize + bytesRead);
        if (payload == NULL) {
            printf("ERROR: Could not allocate memory.\n");
            CloseHandle(hFile);
            exit(1);
        }
        memcpy(payload + payloadSize, chunk, bytesRead);
        payloadSize += bytesRead;
    }
    if (payloadSize == 0) {
        printf("ERROR: Could not read any data from %s.\n", filename);
        CloseHandle(hFile);
        exit(1);
    }

    printf("Payload size: %d.", payloadSize);
    for (unsigned int i = 0; i < payloadSize; i++) {
        if (i % 32 == 0) printf("\n");
        printf("%02x ", payload[i] & 0xff);
    }
    printf("\n\n");
    CloseHandle(hFile);

    // Prepare executable buffer

    void* execBuffer = VirtualAlloc(0, payloadSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(execBuffer, payload, payloadSize);
    free(payload);

    // Execute shellcode

    ((void(*)())execBuffer)();

    return 0;
}
