#ifndef PARTITION_H
#define PARTITION_H

#include <iostream>
#include <string.h>

using namespace std;

class Partition
{
    public:
        Partition();
        char part_status = '0';
        char part_type = 'P';
        char part_fit = 'W';
        int part_start = -1;
        int part_size = -1;
        char part_name[16];
        

    private:
};

#endif // PARTITION_H