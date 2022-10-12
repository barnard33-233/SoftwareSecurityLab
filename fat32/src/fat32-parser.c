#include "sector.h"
#include "DBR.h"
#include "type.h"
#include "file.h"
#include "input.h"

//TODO: mark this is linux
// #include <unistd.h>
// #include <getopt.h>

// const char * dest_default = "~/out";

char * path = "/home/tang_/develop/SoftwareSecurityLab/fat32/virtual.img"; //temp
char * root;
char * target = "RANDOM.TXT";
char * dest = "/home/tang_/develop/SoftwareSecurityLab/fat32/out.txt";

// struct option long_opts[]{
//   // {"root",          required_argument, (int*)root, 'r'},
//   // {"path",          required_argument, (int*)path, 'P'},
//   {"relative_path", required_argument, (int*)target, 'p'},
//   {"image",         required_argument, (int*)path, 'i'}
//   //{}
// }

// inline void parseInput(int argc, char ** argv){
//   int res;
//   for(EOF == (res = getopt_long(argc, argv, ))){

//   }
// }

void PrintBPB(struct DBR_info BPB){
  // printf("%hu\n", BPB.byte_per_sector);
  printf("sector/cluster : %hu\n", BPB.sector_per_cluster);
  printf("reserved sector:%hu\n", BPB.reserved_sector_cnt);
  printf("FAT number     : %hd\n", BPB.FAT_cnt);
  // printf("%d\n", BPB.partition_sector_cnt);
  printf("sector/FAT     :%d\n", BPB.sector_per_FAT);
  printf("root clus      :%d\n", BPB.BPR_root_clus);
}


int main(int argc, char ** argv){
  // TODO:输入解析
  initReadSector(path);

  struct DBR_info bpb = parseBPB();
  PrintBPB(bpb); 
  
  struct Cluster * root_clus_chain = getClusterChain(bpb, bpb.BPR_root_clus);
  
  struct Cluster * ptr = root_clus_chain;
  // DEBUG:
  while(ptr != NULL){
    printf("clusnum:%x\n", ptr->cluster_no);
    uint32 sectornum = ClusterNum2SectorNum(bpb, ptr->cluster_no);
    printf("sectornum:%x\n", sectornum);
    ptr = ptr->next_cluster;
  }
  uint8 *root_data = NULL;
  int root_sector_cnt;
  getClusterChainData(bpb, root_clus_chain, &root_data, &root_sector_cnt);
  printf("root sector count:%d\n", root_sector_cnt);

  for(int i = 0; i < root_sector_cnt * SECTOR_SIZE; i++){
    printf("%02hhx ", root_data[i]);
    if((i + 1) % 16 == 0){
      printf("\n");
    }
  }
  
  struct Filename target_fn = cutFilename(target);
  struct SFNEntry sfn =  traverseRoot(root_data, root_sector_cnt, target_fn);
  displayFile16(bpb, sfn);
  copyFile(bpb, sfn, dest);
  exitReadSector();
  return 0;
}