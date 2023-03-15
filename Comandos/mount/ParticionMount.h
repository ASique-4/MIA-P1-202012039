#ifndef PARTICIONMOUNT_H
#define PARTICIONMOUNT_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>

using namespace std;

class ParticionMount
{
    public:
        string id;
        char name[16];
        string path;
        ParticionMount *siguiente = NULL;
        ParticionMount *anterior = NULL;
        time_t mount_time;
        time_t umount_time;
        int mnt_count;
        bool montada = false;
    private:
};

#endif // PARTICIONMOUNT_H