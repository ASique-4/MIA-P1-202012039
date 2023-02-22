#ifndef RMDISK_H
#define RMDISK_H

#include <iostream>

using namespace std;

class rmdisk
{
    public:
        rmdisk();
        string path = "";
        void make_rmdisk(rmdisk *disco);
    private:
};

#endif // RMDISK_H