#include <Windows.h>
#include "shellcode.h"
#include <winternl.h>
#include <winnt.h>
#include <assert.h>

//----------config----------
#pragma code_seg(".shllcd")
//----------debug-----------
#define NDEBUG
#ifndef NDEBUG
#include <stdio.h>
#define INFO(...) printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

//
#define KERNEL32_NAME_LEN 12
// extern WCHAR kernel32_dll_name[KERNEL32_NAME_LEN + 1];

#define CREATEFILEA_NAME_LEN 11
// extern char createfilea_name[CREATEFILEA_NAME_LEN + 1];
#define WRITEFILE_NAME_LEN 9
// extern char writefile_name[WRITEFILE_NAME_LEN + 1];
#define READFILE_NAME_LEN 8
// extern char readfile_name[READFILE_NAME_LEN + 1];

#define MAX_BUFFER_SIZE 1024
// extern char data[MAX_BUFFER_SIZE];


// extern char target_file[];



//code
BOOL Compare(void* str1, void* str2, int length, int data_width){
    char * _str1 = (char*)str1;
    char * _str2 = (char*)str2;
    for(int i = 0; i < length * data_width; i ++) {
        if(_str1[i] != _str2[i]){
            return FALSE;
        }
    }
    return TRUE;
}

PVOID get_dll(WCHAR * dllname, int dllname_len){
    void * in_memory_order_list;
    __asm{
        mov edx, fs:[0x30]
        mov edx, [edx + 0x0c]
        mov edx, [edx + 0x0c]
        mov in_memory_order_list, edx
    }
    LDR_DATA_TABLE_ENTRY * table = in_memory_order_list;
    while(1){
        INFO("name:%ls\n", table->FullDllName.Buffer);
        BOOL same = Compare(
            ((UNICODE_STRING*)(table->Reserved4))->Buffer,
            dllname,
            dllname_len,
            2
        );
        if(same){
            return table->DllBase;
        }
        table = ((LIST_ENTRY*)(table->Reserved1))->Flink;
        if(table == in_memory_order_list){
            return NULL;
        }
    }
    return NULL;
}

PVOID get_api(PVOID dll_base, CHAR* api_name, int api_name_len){
    IMAGE_DOS_HEADER * dos_header = dll_base;
    INFO("%d\n", dos_header->e_lfanew);
    IMAGE_NT_HEADERS32 * nt_header = (char * )dll_base + dos_header->e_lfanew;
    DWORD export_table_rva = nt_header->OptionalHeader.DataDirectory[0].VirtualAddress;
    IMAGE_EXPORT_DIRECTORY* export_table_va = (char*)dll_base + export_table_rva; // FIXME

    DWORD number_of_names = export_table_va->NumberOfNames;
    DWORD* address_table_va = (char*)dll_base + export_table_va->AddressOfFunctions;
    DWORD* name_pointer_table_va = (char*)dll_base + export_table_va->AddressOfNames;
    WORD* order_table_va = (char*)dll_base + export_table_va->AddressOfNameOrdinals;
    INFO("number of names: %d\n", number_of_names);
    INFO("%s\n", (char*)((char*)dll_base + export_table_va->Name)); //check if it is right

    for(int i = 0; i < number_of_names; i++){
        char* name_va = name_pointer_table_va[i] + (char*)dll_base;

        BOOL response = Compare(
            name_va,
            api_name,
            api_name_len,
            1
        );
        INFO("%s\n", name_va);


        if(response == TRUE){
            WORD order = order_table_va[i];
            PROC api_va = (char*)dll_base + address_table_va[order];

            return api_va;
        }
    }
    return NULL;
}


//entry:
__declspec(dllexport)
void shellcode(){
    WCHAR kernel32_dll_name[KERNEL32_NAME_LEN + 1] = {
        L'K', L'E', L'R', L'N', L'E', L'L', L'3', L'2', L'.', L'D', L'L', L'L', L'\0'
    };

    char createfilea_name[CREATEFILEA_NAME_LEN + 1] = {'C','r','e','a','t','e','F','i','l','e','A',0};
    char writefile_name[WRITEFILE_NAME_LEN + 1] = {'W','r','i','t','e','F','i','l','e',0};
    char readfile_name[READFILE_NAME_LEN + 1] = {'R','e','a','d','F','i','l','e',0};
    char target_file[10] = {'.','\\','n','e','w','.','t','x','t',0};
    char data[14] = {'2','0','2','0','3','0','2','1','8','1','0','2','1',0};
    PVOID base = get_dll(kernel32_dll_name, KERNEL32_NAME_LEN);
    INFO("base: %p\n", base);
    
    // get api in kernel32.dll
    fp_CreateFileA fn_CreateFileA = get_api(base, createfilea_name, CREATEFILEA_NAME_LEN);
    fp_WriteFile fn_WriteFile = get_api(base, writefile_name, WRITEFILE_NAME_LEN);


    HANDLE fh = INVALID_HANDLE_VALUE;
    INFO("Create\n");
    fh = fn_CreateFileA(
        target_file,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if(fh == INVALID_HANDLE_VALUE) INFO("invalid handle\n");
    INFO("Write\n");
    DWORD size_written;
    int ret = fn_WriteFile(
        fh,
        data,
        13,
        &size_written,
        NULL
    );
    INFO("%d, %d\n", size_written, ret);
    INFO("FINISH\n");
}

//----------debug----------
# pragma code_seg(push, r1, ".text")
int main(){
    INFO("start test\n");
    shellcode();
    INFO("End test\n");
    return 0;
}