#ifndef __SHELLCODE_H
#define __SHELLCODE_H

#include <minwindef.h>
#include <fileapi.h>
#include <heapapi.h>

// from fileapi.h

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

typedef BOOL (WINAPI *fp_FindClose)(
    _Inout_ HANDLE hFindFile
    );

typedef HANDLE (WINAPI * fp_FindFirstFileA)(
    _In_ LPCSTR lpFileName,
    _Out_ LPWIN32_FIND_DATAA lpFindFileData
    );

typedef BOOL (WINAPI * fp_FindNextFileA)(
    _In_ HANDLE hFindFile,
    _Out_ LPWIN32_FIND_DATAA lpFindFileData
    );

typedef BOOL (WINAPI *fp_GetFileSizeEx)(
    _In_ HANDLE hFile,
    _Out_ PLARGE_INTEGER lpFileSize
    );

typedef DWORD (WINAPI* fp_SetFilePointer)(
    _In_ HANDLE hFile,
    _In_ LONG lDistanceToMove,
    _Inout_opt_ PLONG lpDistanceToMoveHigh,
    _In_ DWORD dwMoveMethod
    );

typedef BOOL (WINAPI *fp_SetEndOfFile)(
    _In_ HANDLE hFile
    );



// from heapapi.h
typedef LPVOID (WINAPI * fp_HeapAlloc)(
    _In_ HANDLE hHeap,
    _In_ DWORD dwFlags,
    _In_ SIZE_T dwBytes
    );

typedef HANDLE (WINAPI *fp_GetProcessHeap)(
    VOID
    );
// WinUser.h:
typedef int (WINAPI *fp_MessageBoxA)(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType);
#endif