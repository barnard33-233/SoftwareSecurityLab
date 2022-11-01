#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <assert.h>
#include <Windows.h>

#include "pe.h"
#include "type.h"
#include "infect.h"

HANDLE file_handle;

static inline int OpenfileRW(char* file_name){
    file_handle = CreateFile(
        file_name,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    return file_handle != INVALID_HANDLE_VALUE;
}

static inline int ReadWholeFile(HANDLE file_handle, u8 ** pbuffer){
    u8 * buffer;
    LARGE_INTEGER size_to_read;
    GetFileSizeEx(file_handle, &size_to_read);
    u32 size = size_to_read.QuadPart;
    u32 size_read;
    buffer = (u8 *) HeapAlloc(GetProcessHeap(), 0, size);
    if (buffer == NULL){
        return 0;
    }
    ReadFile(file_handle, buffer, (u32)size, &size_read, NULL);
    INFO("%d\n", size_read);
    *pbuffer = buffer;
    return 1;
}

int main(){
    char* filename = "a.exe";
    INFO("Debug: Open File %s\n", filename);
    if(!OpenfileRW(filename)){
        printf("\tFail to open\n"); // FIXME: this is a debug info
    }
    INFO("Debug: Read File %s\n", filename);
    if(!ReadWholeFile(file_handle, &file)){
        printf("\tFail to read\n"); // FIXME: this is a debug info
    }
    INFO("Debug: Parse %s\n", filename);
    parse();
#ifndef NDEBUG
    infoParse();
#endif

    HeapFree(GetProcessHeap(), 0, file);
    CloseHandle(file_handle);
    return 0;
}