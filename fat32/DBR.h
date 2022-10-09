#ifndef _DBR_H
#define _DBR_H

#include "sector.h"
#include <stdlib.h>
#include <string.h>

struct DBR_info{
  unsigned short byte_per_sector; // 每扇区字节数
  unsigned char sector_per_cluster; // 每簇扇区数
  unsigned short reserved_sector_cnt; // 保留扇区数
  unsigned char FAT_cnt; // FAT个数
  unsigned int partition_sector_cnt; // 该分区占扇区数
  unsigned int sector_per_FAT; // FAT占用扇区数
  unsigned int BPR_root_clus;
};

struct DBR_info parseBPB(){
  struct Sector * s;
  s = malloc(sizeof(struct Sector));
  ReadSector(0, s);

  struct DBR_info BPB;

  BPB.byte_per_sector = *((unsigned short *)(s->data + 0xb));
  BPB.sector_per_cluster = *(s->data + 0xd);
  BPB.reserved_sector_cnt = *((unsigned short *)(s->data + 0xe));
  BPB.FAT_cnt = *(s->data + 0x10);
  BPB.partition_sector_cnt = *((unsigned int * )(s->data + 0x20));
  BPB.sector_per_FAT = *((unsigned int *)(s->data + 0x24));
  BPB.BPR_root_clus = *((unsigned int *)(s->data + 0x2C));

  return BPB;
};

#endif