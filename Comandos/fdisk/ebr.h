#ifndef EBR_H
#define EBR_H

#include <iostream>
#include <string.h>

using namespace std;

class EBR
{
    public:
        EBR();
        char part_status;
        char part_fit;
        int part_start = -1;
        int part_size = -1;
        int part_next = -1;
        char part_name[16];

    private:
};

#endif // EBR_H