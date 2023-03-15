#ifndef INODO_H
#define INODO_H

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

class Inodo
{
    public:
        Inodo();
        int i_uid;
        int i_gid;
        int i_size;
        time_t i_atime;
        time_t i_ctime;
        time_t i_mtime;
        int i_block[15];
        char i_type;
        int i_perm;
};

#endif // INODO_H