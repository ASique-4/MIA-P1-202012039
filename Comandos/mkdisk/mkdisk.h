#ifndef MKDISK_H
#define MKDISK_H

#include <iostream>

using namespace std;

class mkdisk
{
    public:
        mkdisk();
        int size = 0;
        string path = "";
        string unit = "";
        string fit = "";

        void make_mkdisk(mkdisk *disco);
    private:
};

#endif // MKDISK_H