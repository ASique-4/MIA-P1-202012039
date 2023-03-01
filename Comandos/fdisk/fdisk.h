#ifndef FDISK_H
#define FDISK_H

#include <iostream>

using namespace std;

class fdisk
{
    public:
        fdisk();
        int size = 0;
        string path = "";
        char name[16];
        char unit;
        char type;
        char fit;
        string delete_ = "";
        int add = 0;
        void make_fdisk(fdisk *disco);
    private:
};

#endif // FDISK_H
