#include "sector.h"
#include "DBR.h"
#include "type.h"
#include "file.h"
#include "input.h"
#include <stdlib.h>

#include <unistd.h>
#include <getopt.h>

extern int optind, opterr, optopt;
extern char * optargi;

const char * dest_default = "~/out";

char path[256];
char target[256];
char dest[256];
struct option long_opts[3] = {
  // {"root",          required_argument, (int*)root, 'r'},
  // {"path",          required_argument, (int*)path, 'P'},
  {"relative_path", required_argument, NULL, 'p'},
  {"image",         required_argument, NULL, 'i'},
  {"output",        required_argument, NULL, 'o'}
  //,{}
};

char * opts = "p:i:o:";

// macros of cmd options:
#define HAVE_P 0x1
#define HAVE_I 0x2
#define HAVE_O 0x4

void parseInput(int argc, char ** argv){
  int res;
  int index;
  char opt_flag = 0;
  while(EOF != (res = getopt_long(argc, argv, opts, long_opts, &index))){

    switch (res){
      case 'o':
      opt_flag &= HAVE_O;
      // printf("o:%s", optarg);
      realpath(optarg, dest);

      break;
      
      case 'p':
      opt_flag &= HAVE_P;
      // printf("p:%s", optarg);
      strcpy(target, optarg);
      break;

      case 'i':
      opt_flag &= HAVE_I;
      // printf("i:%s", optarg);
      realpath(optarg, path);
      break;

      case '?':
      exit(0);
      
      default:break;
    }
  }
}


void PrintBPB(struct DBR_info BPB){
  // printf("%hu\n", BPB.byte_per_sector);
  printf("sector/cluster : %hu\n", BPB.sector_per_cluster);
  printf("reserved sector:%hu\n", BPB.reserved_sector_cnt);
  printf("FAT number     : %hd\n", BPB.FAT_cnt);
  // printf("%d\n", BPB.partition_sector_cnt);
  printf("sector/FAT     :%d\n", BPB.sector_per_FAT);
  printf("root clus      :%d\n", BPB.BPR_root_clus);
}

void PrintSFN(struct SFNEntry sfn){
  char file_name[9];
  char ext[4];
  strncpy(file_name, (char*)(sfn.file_name), 8);
  strncpy(ext, (char*)(sfn.ext_name), 4);
  ext[3] = '\0';
  file_name[8] = '\0';
  stripeSpace(file_name, strlen(file_name));
  stripeSpace(ext, strlen(ext));

  printf("## short file name entry:\n");
  printf("file name  : %s\n", file_name);
  printf("ext name   : %s\n", ext);
  printf("attr       : %02hhx\n", sfn.attr);
  printf("create time: %d, %d, %d\n",
  sfn.create_date, sfn.create_t, sfn.create_t_ms);
  printf("clus no    : %x\n", (uint32)(sfn.clus_no_low) + ((uint32)(sfn.clus_no_high) << 16));
  printf("modify time: %d, %d\n", sfn.modify_date, sfn.modify_t);
  printf("file length: %d\n", sfn.file_len);
  printf("\n");
}

int main(int argc, char ** argv){
  parseInput(argc, argv);
  // printf("%s\n", path);
  // printf("%s\n", target);
  // printf("%s\n", dest);
  // exit(0);

  initReadSector(path);

  struct DBR_info bpb = parseBPB();
  // PrintBPB(bpb); 
  
  struct Cluster * root_clus_chain = getClusterChain(bpb, bpb.BPR_root_clus);
  
  uint8 *root_data = NULL;
  int root_sector_cnt;
  getClusterChainData(bpb, root_clus_chain, &root_data, &root_sector_cnt);
  // printf("root sector count:%d\n", root_sector_cnt);

  // for(int i = 0; i < root_sector_cnt * SECTOR_SIZE; i++){
  //   printf("%02hhx ", root_data[i]);
  //   if((i + 1) % 16 == 0){
  //     printf("\n");
  //   }
  // }
  
  struct Filename target_fn = cutFilename(target);
  struct SFNEntry sfn =  traverseRoot(root_data, root_sector_cnt, target_fn);

  PrintSFN(sfn);

  //displayFile16(bpb, sfn);
  copyFile(bpb, sfn, dest);
  exitReadSector();
  return 0;
}