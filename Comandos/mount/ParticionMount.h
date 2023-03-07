#ifndef PARTICIONMOUNT_H
#define PARTICIONMOUNT_H

#include <iostream>
#include <string.h>
#include "../fdisk/partition.h"

using namespace std;

class ParticionMount
{
    public:
        string id;
        Partition *particion;
        ParticionMount *siguiente = NULL;
        ParticionMount *anterior = NULL;
    private:
};

#endif // PARTICIONMOUNT_H