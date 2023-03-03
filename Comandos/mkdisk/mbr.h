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
        void OrdenarParticiones();

    private:
};

#endif // MBR_H

/*
*
* Una función que busca una partición por su id.
* @return La partición.
*
*/
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

/*
*
* Una función que busca una partición por su nombre.
* @return La partición si la encuentra, NULL si no la encuentra.
*
*/
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

/*  
*
*   Ordena las particiones por su posición de inicio
*   de menor a mayor
*
*/
void MBR::OrdenarParticiones(){
    Partition aux;
    try {
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                if (getPartition(i).part_start < getPartition(j).part_start){
                    aux = getPartition(i);
                    getPartition(i) = getPartition(j);
                    getPartition(j) = aux;
                }
            }
        }
    } catch (exception e){
        return;
    }
}