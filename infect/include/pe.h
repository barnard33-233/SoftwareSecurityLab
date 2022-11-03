/*
 * all about PE format. To replace winnt.h
 */

#ifndef __PE_H
#define __PE_H

#include "type.h"

/*
 * 这个文件中的部分宏的命名规则 : 数据结构__属性_OFF (内部不同单词之间用‘_’隔开)
*/

/* DOS HEADER */
#define DOS_HEADER__NT_HEADER_OFF 0x3c // DOS头中记录的 PE header 偏移的位置

extern u32 NT_header_off;


/* NT Header */
#define NT_HEADER__FILE_HEADER_OFF 0x4

#define NT_HEADER__OPTIONAL_HEADER_OFF 0x18


/* File Header */
#define FILE_HEADER__NUMBER_OF_SECTION_OFF 0x02
extern u16 number_of_sections_old;

#define FILE_HEADER__SIZE_OF_OPTIONAL_HEADER 0x10
extern u16 size_of_optional_header;

/* Optional Header */
struct DataDirectory{ // copy from winnt.h
    u32 VirtualAddress;
    u32 Size;
};

#define OPTIONAL_HEADER__ADDRESS_OF_ENTRY_POINT_OFF 0x10
extern u32 address_of_entry_point_old;

#define OPTIONAL_HEADER__IMAGE_BASE_OFF 0x1c
extern u32 image_base;

#define OPTIONAL_HEADER__SECTION_ALIGNMENT_OFF 0x20
extern u32 section_alignment;

#define OPTIONAL_HEADER__FILE_ALIGNMENT_OFF 0x24
extern u32 file_alignment;

#define OPTIONAL_HEADER__SIZE_OF_IMAGE_OFF 0x38
extern u32 size_of_image_old;

/* Section Header */
#define SECTION_HEADER_SIZE 0x28

#define IMAGE_SIZEOF_SHORT_NAME 8

struct SectionHeader { // copy from winnt.h
    u8    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        u32   PhysicalAddress;
        u32   VirtualSize;
    } Misc;
    u32 VirtualAddress;
    u32 SizeOfRawData;
    u32 PointerToRawData;
    u32 PointerToRelocations;
    u32 PointerToLinenumbers;
    u16 NumberOfRelocations;
    u16 NumberOfLinenumbers;
    u32 Characteristics;
};

extern struct SectionHeader * section_headers;

extern u32 section_tables_start;
extern u32 section_tables_end_old;

#ifndef NDEBUG
#define InfoSectionHeader(h) \
    printf("header info:\n"); \
    printf("\tName: %s\n", h.Name); \
    printf("\tVirtualSize: %d\n", h.Misc.VirtualSize); \
    printf("\tVirtualAddress: 0x%x\n", h.VirtualAddress); \
    printf("\tSizeOfRawData: %d\n", h.SizeOfRawData); \
    printf("\tPointerToRawData: 0x%x\n", h.PointerToRawData); \
    printf("\tPointerToRelocations: 0x%x\n", h.PointerToRelocations); \
    printf("\tPointerToLinenumbers: 0x%x\n", h.PointerToLinenumbers); \
    printf("\tNumberOfRelocations: %d\n", h.NumberOfRelocations); \
    printf("\tNumberOfLinenumbers: %d\n", h.NumberOfLinenumbers); \
    printf("\tCharacteristics: 0x%x\n", h.Characteristics)
#else
#define InfoSectionHeader(h)
#endif
/* Sections */

extern u32 sections_start;
extern u32 sections_end_old;


#endif