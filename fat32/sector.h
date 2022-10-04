#ifndef _SECTOR_H
#define _SECTOR_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define THIS_IS_LINUX
// #define THIS_IS_WINDOWS

#define SECTOR_SIZE 512

struct Sector{
    int sector_num;
    char data[SECTOR_SIZE];
};

// initReadSector
int ReadSector(int sector_num, struct Sector * sector);
void exitReadSector();


// implement:

void PrintSector(struct Sector * sector){
    
    printf("\nSector number: %d\n", sector->sector_num);

    for (int i = 0; i < SECTOR_SIZE; i++){
        printf("%02hhx ", sector->data[i]);
        if ((i + 1) % 16 == 0){
            printf("\n");
        }
    }
    
}


// Windows 系统相关
#ifdef THIS_IS_WINDOWS
#endif

// Linux 系统相关
#ifdef THIS_IS_LINUX

// run in root privelege

void initReadSector(char* disk_path);

FILE * disk_fp;

void initReadSector(char* disk_path){
    disk_fp = fopen(disk_path, "r");
}

int ReadSector(int sector_num, struct Sector * s){
    s->sector_num = sector_num;
    if(disk_fp == NULL){
        return -1;
    }
    assert(disk_fp != NULL);

    fseek(disk_fp, sector_num * SECTOR_SIZE, SEEK_SET);
    fread(s->data, SECTOR_SIZE, 1, disk_fp);
    return 0;
}

void exitReadSector(){
    if(disk_fp != NULL){
        fclose(disk_fp);
    }
}

#endif

#endif