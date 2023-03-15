#ifndef SUPERBLOQUE_H
#define SUPERBLOQUE_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>

using namespace std;

class SuperBloque
{
    public:
        SuperBloque();
        // Guarda el número que identifica el sistema de archivos.
        int s_filesystem_type;
        // Guarda el número total de inodos.
        int s_inodes_count;
        // Guarda el número total de bloques.
        int s_blocks_count;
        // Guarda el número de bloques libres.
        int s_free_blocks_count;
        // Guarda el número de inodos libres.
        int s_free_inodes_count;
        // Guarda la fecha y hora de montaje.
        time_t s_mtime;
        // Guarda la fecha y hora de desmontaje.
        time_t s_umtime;
        // Guarda el número de veces que se ha montado.
        int s_mnt_count;
        // Guarda el valor que identifica el sistema de archivos.
        int s_magic;
        // Tamano del inodo.
        int s_inode_size;
        // Tamano del bloque.
        int s_block_size;
        // Primer inodo libre.
        int s_first_ino;
        // Primer bloque libre.
        int s_first_blo;
        // Bitmap de inodos.
        int s_bm_inode_start;
        // Bitmap de bloques.
        int s_bm_block_start;
        // El inicio de la tabla de inodos.
        int s_inode_start;
        // El inicio de la tabla de bloques.
        int s_block_start;
};

#endif // SUPERBLOQUE_H