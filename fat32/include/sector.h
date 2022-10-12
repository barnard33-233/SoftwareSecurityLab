/*
 * sector.h
 * 实现从一个磁盘中读特定扇区的操作
 */


#ifndef _SECTOR_H
#define _SECTOR_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "type.h"

// #define THIS_IS_WINDOWS

#define SECTOR_SIZE 512 //byte

struct Sector{
    int sector_num;
    uint8 data[SECTOR_SIZE];
};

extern FILE * disk_fp;

/* Read a sector
 * @ param sector_num 扇区号
 * @ param sector 扇区结构体指针，用于存储读到的扇区的数据。需要预先分配内存
 */
int ReadSector(int sector_num, struct Sector * sector);

/*
 * 退出程序前调用。
 */
void exitReadSector();

/*
 * 16进制形式输出扇区数据
 */
void PrintSector(struct Sector * sector);


/*
 * 程序开始时调用
 */
void initReadSector(char* disk_path);


void initReadSector(char* disk_path);

int ReadSector(int sector_num, struct Sector * s);

void exitReadSector();

// TODO:
// // Windows 系统相关
// #ifdef THIS_IS_WINDOWS
// #endif

#endif