#include "file.h"

unsigned int ClusterNum2SectorNum(struct DBR_info bpb, unsigned int cluster_num){
  unsigned int data_first_sec = bpb.reserved_sector_cnt + bpb.FAT_cnt * bpb.sector_per_FAT;
  return data_first_sec + cluster_num * (unsigned int)(bpb.sector_per_cluster);
}

static unsigned int getCluster(struct DBR_info bpb, struct Cluster * c, unsigned int cluster_num){
  c = malloc(struct Cluster);
  c->cluster_no = cluster_num;
  c->next_cluster = NULL;
  c->data = malloc(SECTOR_SIZE * bpb.sector_per_cluster);
  unsigned int sector_num = ClusterNum2SectorNum(bpb, cluster_num);
  for (int i = 0; i < bpb.sector_per_cluster; i++) {
    struct Sector s;
    ReadSector(i, &s);
    memcpy(c->data + (i * SECTOR_SIZE), s.data, SECTOR_SIZE);
  }

  unsigned int FAT1_first_sec = bpb.reserved_sector_cnt;
  unsigned int sector_num_off = cluster_num * 4 / SECTOR_SIZE;
  struct Sector FAT_data;
  ReadSector(FAT1_first_sec + sector_num_off, &FAT_data);
  unsigned int next_cluster_num = *((unsigned int)(FAT_data.data + (cluster_num * 4 % SECTOR_SIZE )));
  return next_cluster_num;
}

struct Cluster * getClusterChain(struct DBR_info bpb, unsigned int first_cluster_num){
  struct Cluster * first_clus;
  struct Cluster * this_clus;
  struct Cluster * last_clus;
  unsigned int clus_num;

  clus_num = getCluster(bpb, this_clus, first_clus);
  first_clus = this_clus;
  last_clus = this_clus;

  while(clus_num != 0xffff){
    clus_num = getCluster(bpb, this_clus, clus_num);
    last_clus->next_cluster = this_clus;
    last_clus = this_clus;
  }

  return first_clus;  
}

// TODO: debug this file
// TODO: detect File structure