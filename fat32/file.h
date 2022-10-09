#ifndef _FILE_H
#define _FILE_H

#include "DBR.h"

struct Cluster{
    unsigned int cluster_no;
    struct Cluster * next_cluster;
    char * data;
};



unsigned int ClusterNum2SectorNum(struct DBR_info bpb, unsigned int cluster_num);

struct Cluster * getClusterChain(unsigned int first_cluster_num);

#endif