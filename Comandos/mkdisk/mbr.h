#ifndef MBR_H
#define MBR_H

#include <iostream>
#include <string.h>
#include <ctime>
#include "../fdisk/partition.h"

using namespace std;

Partition::Partition(){}
class MBR
{
    public:
        MBR();
        int mbr_tamano;
        time_t mbr_fecha_creacion;
        int mbr_disk_signature;
        char mbr_disk_fit;
        Partition mbr_partition_1;
        Partition mbr_partition_2;
        Partition mbr_partition_3;
        Partition mbr_partition_4;

        Partition getPartition(int index);
        Partition* findPartition(char name[16]);

    private:
};

#endif // MBR_H

Partition MBR::getPartition(int index){
    switch(index){
        case 1:
            return mbr_partition_1;
        case 2:
            return mbr_partition_2;
        case 3:
            return mbr_partition_3;
        case 4:
            return mbr_partition_4;
        default:
            return Partition();
    }
}

Partition* MBR::findPartition(char name[16]){
    if (strcmp(mbr_partition_1.part_name, name) == 0){
        return &mbr_partition_1;
    } else if (strcmp(mbr_partition_2.part_name, name) == 0){
        return &mbr_partition_2;
    } else if (strcmp(mbr_partition_3.part_name, name) == 0){
        return &mbr_partition_3;
    } else if (strcmp(mbr_partition_4.part_name, name) == 0){
        return &mbr_partition_4;
    } else {
        return NULL;
    }
}