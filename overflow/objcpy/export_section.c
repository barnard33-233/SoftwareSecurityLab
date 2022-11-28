#include <Windows.h>
#include <winnt.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// cl export_section.c

#define NDEBUG
#ifndef NDEBUG
#define INFO(...) printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

char * usage = "usage: export_section file_name section_name\n";

IMAGE_SECTION_HEADER * p_section_headers;
DWORD image_base;
DWORD number_of_sections;

DWORD rva2fov(DWORD rva){
    int num;
    if(rva < p_section_headers[0].VirtualAddress){
        return rva;
    }
    for(num = 0; num < number_of_sections; num++){
        if(rva < p_section_headers[num].VirtualAddress){
            break;
        }
    }
    num--;
    INFO("\tnum: %d\n", num);
    int off = rva - p_section_headers[num].VirtualAddress;
    INFO("\toff: %x\n", off);
    int fov = off + p_section_headers[num].PointerToRawData;
    INFO("\t%x, %x, %x\n", off, p_section_headers[num].PointerToRawData, fov);
    return fov;
}

int get_sec(char * pe_file, char* section, int * off, char ** buffer, int* size){
    char * _buffer;
    IMAGE_DOS_HEADER * p_dos_header = pe_file;
    IMAGE_NT_HEADERS * p_nt_header = pe_file + p_dos_header->e_lfanew;
    DWORD optional_header_size = p_nt_header->FileHeader.SizeOfOptionalHeader;

    p_section_headers = (char*)&(p_nt_header->OptionalHeader) + optional_header_size;
    image_base = p_nt_header->OptionalHeader.ImageBase;
    number_of_sections = p_nt_header->FileHeader.NumberOfSections;
    INFO("number of sections = %d\n", number_of_sections);
#ifndef NDEBUG
    for(int i = 0; i < number_of_sections; i ++){
        printf("------\n");
        printf("\tname: %s\n", p_section_headers[i].Name);
        printf("\tvirtual size  : %d\n", p_section_headers[i].Misc.VirtualSize);
        printf("\tvirtual address: 0x%x\n", p_section_headers[i].VirtualAddress);
        printf("\tsize of raw data: %d\n", p_section_headers[i].SizeOfRawData);
        printf("\tpointer to raw data: 0x%x\n", p_section_headers[i].PointerToRawData);
    }
#endif

    DWORD export_table_rva = p_nt_header->OptionalHeader.DataDirectory[0].VirtualAddress;
    IMAGE_EXPORT_DIRECTORY* p_export_table = pe_file + rva2fov(export_table_rva);
    DWORD export_entry_rva = p_export_table->AddressOfFunctions;
    INFO("rva: 0x%x\n", export_entry_rva);
    DWORD export_entry_fov = rva2fov(export_entry_rva);
    INFO("fov: 0x%x\n", export_entry_fov);
    DWORD* p_export_entry = pe_file + export_entry_fov;
    // int address_of_entry_point = p_nt_header->OptionalHeader.AddressOfEntryPoint;


    for(int i = 0; i < number_of_sections; i++){
        INFO("found: %s\n", p_section_headers[i].Name);
        if(strncmp(section, p_section_headers[i].Name, strlen(section)) == 0){

            INFO("START COPY\n");
            int section_size = p_section_headers[i].SizeOfRawData;
            _buffer = malloc(section_size);
            assert(_buffer != NULL);
            memcpy(_buffer, pe_file + p_section_headers[i].PointerToRawData, section_size);
            INFO("END COPY\n");

            // find entry
            *size = p_section_headers[i].Misc.VirtualSize;
            // *off = address_of_entry_point - p_section_headers[i].VirtualAddress;
            *off = p_export_entry[0] - p_section_headers[i].VirtualAddress;
            *buffer = _buffer;
            return 0;
        }
    }

    return -1;
}

int main(int argc, char ** argv){
    // argv[1]: file name
    // argv[2]: section name
    if(argc != 3){fputs(usage, stderr);}

    char * file = argv[1];
    char * section = argv[2];
    INFO("file name : %s\n", file);
    INFO("section name: %s\n", section);
    
    HANDLE f = CreateFile(
        file,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if(f == INVALID_HANDLE_VALUE){
        fprintf(stderr, "%s not found\n%s\n", file, usage);
    }
    assert(f != INVALID_HANDLE_VALUE);
    LARGE_INTEGER _size;
    GetFileSizeEx(f, &_size);
    int size = _size.QuadPart;
    INFO("PE file size = %d\n", size);

    char * pe_buffer = malloc(size);
    assert(pe_buffer != NULL);
    assert(_msize(pe_buffer) == size);
    memset(pe_buffer, 0, size);

    int actually_read;
    ReadFile(
        f,
        pe_buffer,
        size,
        &actually_read,
        NULL
    );
    INFO("actually, read %d bytes\n", actually_read);
    CloseHandle(f);
    INFO("check if this is a pe file:%c%c\n", pe_buffer[0], pe_buffer[1]);

    char* out_name = malloc(strlen(section) + 5);
    strcpy(out_name, section);
    strcat(out_name, ".bin");

    char * section_data; // allocate in get_sec
    int off;
    int section_size;
    INFO("--------START-GET-SECTION--------\n");
    int response = get_sec(pe_buffer, section, &off, &section_data, &section_size);
    INFO("--------END-GET-SECTION--------\n");
    if(response != 0){
        printf("no such section\n");
        return 0;
    }

    printf("int off = %d;\n", off);
    HANDLE of = CreateFile(
        out_name,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    int size_written;
    WriteFile(
        of,
        section_data,
        section_size,
        &size_written,
        NULL
    );
    CloseHandle(of);

    free(pe_buffer);
    free(out_name);
    free(section_data);
    return  0;
}