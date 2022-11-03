#ifndef __INFECT_H
#define __INFECT_H

#include <stdio.h>
#include "type.h"

int parse();
void AdjustINjection(u8 injection[]);
void infect(u8 injection[], HANDLE file_handle);

extern u8 * file;
extern u8 injection[];

extern u32 address_of_entry_point_new;

#ifndef NDEBUG
#define infoParse() \
    printf("DOS HEADER:\n\tNT header offset: 0x%x\n", NT_header_off);\
    printf("NT HEADER:\n\tnumber of sections: %hd\n\tsize of image: %hd\n\t"\
           "address of entry point: 0x%x\n\tsize of optional header: %hd\n\t"\
           "section alignment: 0x%x\n\tfile alignment: 0x%x\n",\
           number_of_sections_old, size_of_image_old, address_of_entry_point_old, size_of_optional_header,\
           section_alignment, file_alignment);\
    printf("Section Tables:\n\tstart: 0x%x\n\tend: 0x%x\n", section_tables_start, section_tables_end_old);\
    printf("Sections:\n\tstart: 0x%x\n\tend: 0x%x\n", sections_start, sections_end_old)
#else
#define infoParse()
#endif

#ifndef NDEBUG
#define INFO(...) printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

#endif
