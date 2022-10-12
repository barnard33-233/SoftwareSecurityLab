#ifndef _FILE_H
#define _FILE_H

#include "DBR.h"
#include "type.h"
#include "input.h"

#define FNE_SIZE 32 //byte

#define ERROR 0xffffffff
#define NO_SUCH_FILE 0xfffffffe

#pragma pack(1)

struct Cluster{
  uint32 cluster_no;
  struct Cluster * next_cluster;
};

struct SFNEntry{
  uint8 file_name[8];
  uint8 ext_name[3];
  // 0xb:
  uint8 attr;
  uint8 reserved;
  uint8 create_t_ms;
  uint16 create_t;
  uint16 create_date;
  uint16 visit_date;
  uint16 clus_no_high;
  uint16 modify_t;
  uint16 modify_date;
  uint16 clus_no_low;
  uint32 file_len;
};

struct LFNEntry{
  uint8 attr;
  uint8 name_1[10]; // 1--5
  // 0xb:
  uint8 sign;
  uint8 reserved;
  uint8 check;
  uint8 name_2[12]; // 6--11
  uint16 first_clus;
  uint8 name_3[4]; // 12--13
};

uint32 ClusterNum2SectorNum(struct DBR_info bpb, uint32 cluster_num);

struct Cluster * getClusterChain(struct DBR_info bpb, uint32 first_cluster_num);

void getClusterChainData(struct DBR_info bpb, struct Cluster * cluster_chain, uint8 ** data, int* sector_cnt);

struct SFNEntry traverseRoot(uint8 * data, int sector_cnt, struct Filename target);

int copyFile(struct DBR_info bpb, struct SFNEntry sfn, char * dest);

void displayFile16(struct DBR_info bpb, struct SFNEntry sfn);

#endif