#ifndef __SHELLCODE_H
#define __SHELLCODE_H

#include <minwindef.h>
typedef struct FuncTable{
    void* fn;
    char* name;
}FuncTable;

typedef struct DllTable{
    WCHAR* name;
    FuncTable fn_table[16];
}DllTable;

// ! 这里需要注意，因为 shellcode 在 32 位下编译，这里的函数使用的都不是stdcall

typedef BOOL(WINAPI *fp_CloseHandle)(HANDLE);

typedef BOOL(WINAPI *fp_WriteFile)(
    _In_ HANDLE hFile,
    _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
    _In_ DWORD nNumberOfBytesToWrite,
    _Out_opt_ LPDWORD lpNumberOfBytesWritten,
    _Inout_opt_ LPOVERLAPPED lpOverlapped
    );
typedef BOOL (WINAPI *fp_ReadFile)(
    _In_ HANDLE hFile,
    _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer,
    _In_ DWORD nNumberOfBytesToRead,
    _Out_opt_ LPDWORD lpNumberOfBytesRead,
    _Inout_opt_ LPOVERLAPPED lpOverlapped
    );

typedef HANDLE (WINAPI * fp_CreateFileA)(
    _In_ LPCSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile
    );

#endif