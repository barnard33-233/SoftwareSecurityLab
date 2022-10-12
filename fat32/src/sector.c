#include "sector.h"

FILE * disk_fp;

void PrintSector(struct Sector * sector){
    
    printf("\nSector number: %d\n", sector->sector_num);

    for (int i = 0; i < SECTOR_SIZE; i++){
        printf("%02hhx ", sector->data[i]);
        if ((i + 1) % 16 == 0){
            printf("\n");
        }
    }
    
}

void initReadSector(char* disk_path){
    disk_fp = fopen(disk_path, "r");
}

int ReadSector(int sector_num, struct Sector * s){
    assert(disk_fp != NULL);
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