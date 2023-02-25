#ifndef MBR_H
#define MBR_H

#include <iostream>
#include <string.h>
#include <ctime>
#include "../fdisk/partition.h"

using namespace std;

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

    private:
};

#endif // MBR_H