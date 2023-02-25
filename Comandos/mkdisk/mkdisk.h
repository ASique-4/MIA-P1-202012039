#ifndef MKDISK_H
#define MKDISK_H

#include <iostream>
#include "mbr.h"

using namespace std;

class mkdisk
{
    public:
        mkdisk();
        int size = 0;
        string path = "";
        string name = "";
        char unit;
        char fit;
        MBR mbr;

        void make_mkdisk(mkdisk *disco);
    private:
};

#endif // MKDISK_H