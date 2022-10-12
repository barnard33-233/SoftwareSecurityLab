#include "file.h"
#include "type.h"
#include "input.h"
#include <string.h>

inline uint32 ClusterNum2SectorNum(struct DBR_info bpb, uint32 cluster_num){
  return bpb.reserved_sector_cnt + bpb.FAT_cnt * bpb.sector_per_FAT + (cluster_num - 2) * (uint32)(bpb.sector_per_cluster);
}

struct Cluster * getClusterChain(struct DBR_info bpb, uint32 first_cluster_num){
  uint32 current_FAT_sector_num;
  uint32 FAT_sector_num;
  struct Sector FAT_sector;
  
  struct Cluster * first_cluster;
  struct Cluster * this_cluster;
  struct Cluster * last_cluster;
  uint32 cluster_num;
  uint32 next_cluster_num;

  uint32 cluster_offset;

  FAT_sector_num = (first_cluster_num * 4 / SECTOR_SIZE) + bpb.reserved_sector_cnt;
  ReadSector(FAT_sector_num, &FAT_sector);
  cluster_offset = (first_cluster_num * 4 % SECTOR_SIZE);
  
  this_cluster = malloc(sizeof(struct Cluster));
  this_cluster->cluster_no = first_cluster_num;
  this_cluster->next_cluster = NULL;
  next_cluster_num = *((uint32 *)(FAT_sector.data + cluster_offset));
  first_cluster = this_cluster;

  last_cluster = this_cluster;

  while (next_cluster_num < 0x0ffffff8) {
    if (!(next_cluster_num > 0x2 && next_cluster_num < 0x0fffffef)) {
      fprintf(stderr, "An Error Occurred: something goes wrong in FAT");
      exit(0);
      // TODO better hint to user
    }
    cluster_num = next_cluster_num;
    current_FAT_sector_num = (cluster_num  * 4 / SECTOR_SIZE) + bpb.reserved_sector_cnt;
    if (current_FAT_sector_num != FAT_sector_num) {
      ReadSector(current_FAT_sector_num, &FAT_sector);
      FAT_sector_num = current_FAT_sector_num;
    }

    cluster_offset = (cluster_num * 4) % SECTOR_SIZE;

    this_cluster = malloc(sizeof(struct Cluster));
    this_cluster->cluster_no = cluster_num;
    this_cluster->next_cluster = NULL;
    next_cluster_num = *((uint32 *)(FAT_sector.data + cluster_offset));
    last_cluster->next_cluster = this_cluster;
    last_cluster = this_cluster;
  }
  
  return first_cluster;
}

void getClusterChainData(struct DBR_info bpb, struct Cluster * cluster_chain, uint8** data_res, int* sector_cnt){
  uint8 * data;

  int allocated_size = SECTOR_SIZE * bpb.sector_per_cluster;
  *sector_cnt = 0;
  data = malloc(allocated_size);

  struct Cluster * clus_ptr = cluster_chain;
  while(clus_ptr != NULL) {
    if (*sector_cnt >= allocated_size) {
      allocated_size *= 2;
      data = realloc(data, allocated_size);
    }
    int sector_num = ClusterNum2SectorNum(bpb, clus_ptr->cluster_no);
    for(int i = 0; i < bpb.sector_per_cluster; i++){
      struct Sector sector;
      ReadSector(sector_num + i, &sector);
      memcpy(data + (*sector_cnt * SECTOR_SIZE) ,sector.data, SECTOR_SIZE);
      (*sector_cnt) ++;
      assert((*sector_cnt) * SECTOR_SIZE <= allocated_size);
    }
    clus_ptr = clus_ptr->next_cluster;
  }

  (*data_res) = data;
}

static inline int 
buildFNE(uint8 * data, int boundary, struct SFNEntry * sfn, struct LFNEntry** lfn_res, int * cnt_lfn_res){
  struct LFNEntry * lfn = malloc(sizeof(struct LFNEntry));
  int cnt_lfn = 0;
  int allocated_lfn_size = 1;

  for(int i = 0;i < boundary;i++){
    uint8 * start = data + i * 32; // FAT32

    switch (start[0xb]) {
    
    case 0xf:
      if(cnt_lfn >= allocated_lfn_size) {
        lfn = realloc(lfn, 2 * allocated_lfn_size * sizeof(struct LFNEntry));
      }
      lfn[cnt_lfn] = *((struct LFNEntry *)start);
      cnt_lfn++;
      break;
    
    default:
      *sfn = *((struct SFNEntry *)start);
      goto end;
    }
  }

  return -1;

  end:
  if(cnt_lfn == 0){
    free(lfn);
    lfn = NULL;
  }
  *lfn_res = lfn;
  *cnt_lfn_res = cnt_lfn;
  return 0;
}

static inline char* buildFileName(struct LFNEntry* lfn){
  char * name = malloc(27*sizeof(char));
  memcpy(name, lfn->name_1, 10);
  memcpy(name + 10, lfn->name_2, 12);
  memcpy(name + 22, lfn->name_3, 4);
  name[26] = '\0';
  return name;
}

void printSFN(struct SFNEntry sfn){
  char filename[9];
  char ext[4];
  strncpy(filename, (char *)(sfn.file_name), 8);
  strncpy(ext, (char*)(sfn.ext_name), 3);
  filename[8] = '\0';
  ext[3] = '\0';

  printf("%s\n", filename);
  printf("%s\n", ext);
  printf("clusnum: %hd\n", (uint32)(sfn.clus_no_low) + ((uint32)(sfn.clus_no_high) << 16));
  printf("filesize: %hd\n", sfn.file_len);
}

struct SFNEntry traverseRoot(uint8 * data, int sector_cnt, struct Filename target){
  int fne_cnt = (sector_cnt * SECTOR_SIZE) / 32;
  
  for(int i = 1; i < fne_cnt;){
    struct SFNEntry sfn;
    struct LFNEntry * lfn_list = NULL;
    int cnt_lfn;
    buildFNE(data + i * 32, fne_cnt - i, &sfn, &lfn_list, &cnt_lfn);
    
    // DEBUG:
    printSFN(sfn);
    
    struct Filename this_filename;
    if(cnt_lfn == 0) { // no lfn
      this_filename.name = sfn.file_name;
      this_filename.ext = sfn.ext_name;
    }
    else{ // have lfn
      char * filename = malloc(1 + 26 * cnt_lfn);
      for(int i = 0; i < cnt_lfn; i++){
        char * name = buildFileName(lfn_list + cnt_lfn - i - 1);
        memcpy(filename + i * 26, name, 26);
      }
      filename[26 * cnt_lfn] = '\0';
      
      this_filename = cutFilename(filename);
    }
    
    if(strncmp(this_filename.ext, target.ext, strlen(target.ext)) == 0 &&
      strncmp(this_filename.name, target.name, strlen(target.name)) == 0) {
      return sfn;
    }
    i += (cnt_lfn + 1);
  }
  struct SFNEntry nothing;
  nothing.file_len = 0;
  return nothing;
}

// uint32 traverseDict(uint8 * data, char* dest){

// }

int copyFile(struct DBR_info bpb, struct SFNEntry sfn, char * dest){
  uint32 clus_num = (uint32)(sfn.clus_no_low) + ((uint32)(sfn.clus_no_high) << 16);
  struct Cluster * clus_chain = getClusterChain(bpb, clus_num);

  FILE* dest_fp = fopen(dest, "wb");
  if(dest_fp == NULL){
    return ERROR;
  }
  int rest_file_len = sfn.file_len;

  struct Cluster * clus_ptr = clus_chain;

  while(1) {
    assert(rest_file_len >= 0);
    if(clus_ptr == NULL || rest_file_len == 0){
      break;
    }
    struct Sector sector;
    for(int i = 0; i < bpb.sector_per_cluster; i++){
      uint32 sector_num = ClusterNum2SectorNum(bpb, clus_ptr->cluster_no);
      ReadSector(sector_num + i, &sector);
      if(rest_file_len >= SECTOR_SIZE * bpb.sector_per_cluster){
        assert(dest_fp != NULL);
        fwrite(sector.data, SECTOR_SIZE, 1, dest_fp);
        rest_file_len -= SECTOR_SIZE * bpb.sector_per_cluster;
      }
      else{
        assert(dest_fp != NULL);
        assert(rest_file_len < SECTOR_SIZE);
        fwrite(sector.data, rest_file_len, 1, dest_fp);
        break;
      }
    }
    clus_ptr = clus_ptr->next_cluster;
  }
  return 0;
}

void displayFile16(struct DBR_info bpb, struct SFNEntry sfn){
  uint32 cluster_num = (uint32)(sfn.clus_no_low) + ((uint32)(sfn.clus_no_high) << 16);
  struct Cluster * cluster_chain = getClusterChain(bpb, cluster_num);
  struct Cluster * clus_ptr = cluster_chain;
  
  int rest_file_len = sfn.file_len;
  while(clus_ptr != NULL){
    uint32 sector_num = ClusterNum2SectorNum(bpb, clus_ptr->cluster_no);
    struct Sector sector;
    for(int i = 0; i < bpb.sector_per_cluster; i++){
      ReadSector(sector_num + i, &sector);
      PrintSector(&sector);
      rest_file_len -= SECTOR_SIZE;
      if (rest_file_len <= 0){
        return;
      }
    }
  }
}