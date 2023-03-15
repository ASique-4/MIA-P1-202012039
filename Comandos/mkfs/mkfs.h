#ifndef MKFS_H
#define MKFS_H

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

class MKFS
{
    public:
        MKFS();
        char id[16];
        char type[16] = "Full";
        char fs[16] = "2fs";
        void make_mkfs(MKFS* mkfs, ListaDobleMount* listaMount);
};

#endif // MKFS_H