#include "DBR.h"

struct DBR_info parseBPB(){
  struct Sector * s;
  s = malloc(sizeof(struct Sector));
  ReadSector(0, s);

  struct DBR_info BPB;

  BPB.byte_per_sector = *((uint16 *)(s->data + 0xb));
  BPB.sector_per_cluster = *(s->data + 0xd);
  BPB.reserved_sector_cnt = *((uint16 *)(s->data + 0xe));
  BPB.FAT_cnt = *(s->data + 0x10);
  BPB.partition_sector_cnt = *((uint32 * )(s->data + 0x20));
  BPB.sector_per_FAT = *((uint32 *)(s->data + 0x24));
  BPB.BPR_root_clus = *((uint32 *)(s->data + 0x2C));

  return BPB;
};