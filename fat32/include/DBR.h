#ifndef _DBR_H
#define _DBR_H

#include "sector.h"
#include <stdlib.h>
#include <string.h>

struct DBR_info{
  uint16 byte_per_sector; // 每扇区字节数
  uint8 sector_per_cluster; // 每簇扇区数
  uint16 reserved_sector_cnt; // 保留扇区数
  uint8 FAT_cnt; // FAT个数
  uint32 partition_sector_cnt; // 该分区占扇区数
  uint32 sector_per_FAT; // FAT占用扇区数
  uint32 BPR_root_clus;
};

struct DBR_info parseBPB();


#endif