#ifndef SUPERBLOQUE_H
#define SUPERBLOQUE_H

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

class SuperBloque
{
    public:
        SuperBloque();
        int s_filesystem_type;
        int s_inodes_count;
        int s_blocks_count;
        int s_free_blocks_count;
        int s_free_inodes_count;
        int s_mtime;
        int s_umtime;
        int s_mnt_count;
        int s_magic;
        int s_inode_size;
        int s_block_size;
        int s_first_ino;
        int s_first_blo;
        int s_bm_inode_start;
        int s_bm_block_start;
        int s_inode_start;
        int s_block_start;
};

#endif // SUPERBLOQUE_H