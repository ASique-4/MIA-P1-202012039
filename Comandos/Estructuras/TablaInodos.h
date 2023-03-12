#ifndef TABLAINODOS_H
#define TABLAINODOS_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>

using namespace std;

class TablaInodos
{
    public:
        TablaInodos();
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

#endif // TABLAINODOS_H
