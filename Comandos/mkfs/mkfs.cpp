#include "mkfs.h"
#include "../Estructuras/SuperBloque.h"
#include "../Estructuras/Inodo.h"
#include "../Estructuras/BloqueDeApuntadores.h"
#include "../Estructuras/BloqueDeCarpetas.h"
#include "../mount/ListaDobleMount.h"
#include "../fdisk/partition.h"
#include <math.h>


MKFS::MKFS()
{
}

SuperBloque::SuperBloque()
{
}

Inodo::Inodo()
{
}

BloqueDeCarpetas::BloqueDeCarpetas()
{
}

void crearUsuariosTXT(string path)
{
    FILE *archivo;
    // Quitamos el nombre del disco
    path = path.substr(0, path.find_last_of("/"));
    path += "/usuarios.txt";
    archivo = fopen(path.c_str(), "w");
    if (archivo == NULL)
    {
        cout << "Path: " << path << endl;
        cout << "Error: No se pudo abrir el disco" << endl;
        return;
    }
    string linea = "1,G,root\n";
    fwrite(linea.c_str(), sizeof(char), linea.length(), archivo);
    linea = "1,U,root,root,123\n";
    fwrite(linea.c_str(), sizeof(char), linea.length(), archivo);
    fclose(archivo);
}


/**
 * Crea un nuevo sistema de archivos EXT2 en la partición especificada por el usuario
 * 
 * @param mkfs Esta es la estructura que contiene la información de la partición a formatear.
 * @param listaMount Una lista de todas las particiones montadas.
 * 
 * @return Un puntero a un SuperBlock
 */
void crearExt2(MKFS *mkfs, ListaDobleMount *listaMount)
{
    FILE *archivo;
    MBR mbr;
    archivo = fopen(listaMount->buscar(mkfs->id)->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "Error: No se pudo abrir el disco" << endl;
        return;
    }
    fread(&mbr, sizeof(MBR), 1, archivo);
    fseek(archivo, 0, SEEK_SET);
    Partition* particion;
    particion = mbr.findPartition(listaMount->buscar(mkfs->id)->name);
    // Buscamos el start
    Partition particiones[4] = {mbr.mbr_partition_1, mbr.mbr_partition_2, mbr.mbr_partition_3, mbr.mbr_partition_4};
    int start = 0;
    for (int i = 0; i < 4; i++)
    {
        if (particiones[i].part_status == '1' && strcmp(particiones[i].part_name, listaMount->buscar(mkfs->id)->name) == 0)
        {
            start = particiones[i].part_start;
            break;
        }
    }
    fseek(archivo, start, SEEK_SET);
    // Crear SuperBloque
    SuperBloque* sb = new SuperBloque();
    sb->s_filesystem_type = 2;
    sb->s_inodes_count = floor(particion->part_size - sizeof(SuperBloque)) / (1 + 3 + sizeof(Inodo) + 3 * sizeof(BloqueDeCarpetas));
    sb->s_blocks_count = 3 * sb->s_inodes_count;
    sb->s_free_blocks_count = sb->s_blocks_count - 1;
    sb->s_free_inodes_count = sb->s_inodes_count - 1;
    sb->s_mtime = listaMount->buscar(mkfs->id)->mount_time;
    sb->s_umtime = listaMount->buscar(mkfs->id)->umount_time;
    sb->s_mnt_count = listaMount->buscar(mkfs->id)->mnt_count;
    sb->s_magic = 0xEF53;
    sb->s_inode_size = sizeof(Inodo);
    sb->s_block_size = sizeof(BloqueDeCarpetas);
    sb->s_first_ino = 0;
    sb->s_first_blo = 0;
    sb->s_bm_inode_start = particion->part_start + sizeof(SuperBloque);
    sb->s_bm_block_start = sb->s_bm_inode_start + sb->s_inodes_count;
    sb->s_inode_start = sb->s_bm_block_start + sb->s_blocks_count;
    sb->s_block_start = sb->s_inode_start + sb->s_inodes_count * sizeof(Inodo);

    fwrite(sb, sizeof(SuperBloque), 1, archivo);

    // BitMap de Inodos
    fseek(archivo, sb->s_bm_inode_start, SEEK_SET);
    char* bmInodos = new char[sb->s_inodes_count];
    for (int i = 0; i < sb->s_inodes_count; i++)
    {
        bmInodos[i] = '0';
    }
    bmInodos[0] = '1';
    fwrite(bmInodos, sizeof(char), sb->s_inodes_count, archivo);

    // BitMap de Bloques
    fseek(archivo, sb->s_bm_block_start, SEEK_SET);
    char* bmBloques = new char[sb->s_blocks_count];
    for (int i = 0; i < sb->s_blocks_count; i++)
    {
        bmBloques[i] = '0';
    }
    bmBloques[0] = '1';
    fwrite(bmBloques, sizeof(char), sb->s_blocks_count, archivo);

    // Inodos
    fseek(archivo, sb->s_inode_start, SEEK_SET);
    Inodo* inodos = new Inodo[sb->s_inodes_count];
    for (int i = 0; i < sb->s_inodes_count; i++)
    {
        inodos[i].i_uid = 1;
        inodos[i].i_gid = 1;
        inodos[i].i_size = 0;
        inodos[i].i_atime = listaMount->buscar(mkfs->id)->mount_time;
        inodos[i].i_ctime = listaMount->buscar(mkfs->id)->mount_time;
        inodos[i].i_mtime = listaMount->buscar(mkfs->id)->mount_time;
        inodos[i].i_block[0] = -1;
        inodos[i].i_block[1] = -1;
        inodos[i].i_block[2] = -1;
        inodos[i].i_block[3] = -1;
        inodos[i].i_block[4] = -1;
        inodos[i].i_block[5] = -1;
        inodos[i].i_block[6] = -1;
        inodos[i].i_block[7] = -1;
        inodos[i].i_block[8] = -1;
        inodos[i].i_block[9] = -1;
        inodos[i].i_block[10] = -1;
        inodos[i].i_block[11] = -1;
        inodos[i].i_type = '0';
        inodos[i].i_perm = 664;
    }
    inodos[0].i_type = '1';
    fwrite(inodos, sizeof(Inodo), sb->s_inodes_count, archivo);

    // Bloques
    fseek(archivo, sb->s_block_start, SEEK_SET);
    BloqueDeCarpetas* bloques = new BloqueDeCarpetas[sb->s_blocks_count];
    for (int i = 0; i < sb->s_blocks_count; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            bloques[i].b_content[j].b_inodo = -1;
            strcpy(bloques[i].b_content[j].b_name, "");
        }
    }
    strcpy(bloques[0].b_content[0].b_name, ".");
    bloques[0].b_content[0].b_inodo = 0;
    strcpy(bloques[0].b_content[1].b_name, "..");
    bloques[0].b_content[1].b_inodo = 0;
    fwrite(bloques, sizeof(BloqueDeCarpetas), sb->s_blocks_count, archivo);

    fclose(archivo);
    crearUsuariosTXT(listaMount->buscar(mkfs->id)->path);
}

void crearExt3(MKFS* mkfs, ListaDobleMount* listaMount)
{
    cout << "¡Error! El sistema de archivos 3fs no está implementado aún." << endl;
}


void MKFS::make_mkfs(MKFS* mkfs, ListaDobleMount* listaMount)
{
    if (strcmp(mkfs->fs, "2fs") == 0)
    {
        crearExt2(mkfs, listaMount);
    }
    else if (mkfs->fs == "3fs")
    {
        cout << "¡Error! El sistema de archivos 3fs no está implementado aún." << endl;
    }
    else if (mkfs->fs == "")
    {
        crearExt2(mkfs, listaMount);
    }
    else
    {
        cout << "¡Error! El sistema de archivos " << mkfs->fs << " no está implementado aún." << endl;
    }
}