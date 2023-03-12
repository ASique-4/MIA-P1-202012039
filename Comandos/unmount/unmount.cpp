#include "unmount.h"
#include "../mount/ListaDobleMount.h"
#include "../fdisk/ebr.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

UNMOUNT::UNMOUNT()
{
}

void UNMOUNT::make_unmount(UNMOUNT *disco, ListaDobleMount *lista)
{
    if (disco->id == "")
    {
        cout << "¡Error! No podemos desmontar la partición si no nos dicen cuál es. ¿Alguna pista? " << endl;
        return;
    }
    lista->imprimir();
    try
    {
        ParticionMount *particion = new ParticionMount();
        particion = lista->buscar(disco->id);
        if (particion == NULL)
        {
            cout << "¡Error! No se encontró la partición con el ID: " << disco->id << endl;
            return;
        }
        FILE *archivo = fopen(particion->path.c_str(), "rb+");
        MBR mbr;
        fseek(archivo, 0, SEEK_SET);
        fread(&mbr, sizeof(MBR), 1, archivo);
        
        try
        {
            bool found = false;
            for (int i = 0; i < 4; i++)
            {
                if (strcmp(mbr.getPartition(i).part_name, particion->name) == 0)
                {
                    mbr.findPartition(particion->name)->part_status = '0';
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                for (int i = 0; i < 4; i++)
                {
                    if (mbr.getPartition(i).part_type == 'e')
                    {
                        EBR ebr;
                        fseek(archivo, mbr.getPartition(i).part_start, SEEK_SET);
                        fread(&ebr, sizeof(EBR), 1, archivo);
                        while (ebr.part_next != -1)
                        {
                            if (strcmp(ebr.part_name, particion->name) == 0)
                            {
                                ebr.part_status = '0';
                                found = true;
                                break;
                            }
                            fseek(archivo, ebr.part_next, SEEK_SET);
                            fread(&ebr, sizeof(EBR), 1, archivo);
                        }
                    }
                }
                if (!found)
                {
                    cout << "No se encontró la partición" << endl;
                    return;
                }
            }
        }
        catch (const std::exception& e)
        {
            cout << "No se encontró la partición" << endl;
            return;
        }

        fseek(archivo, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        fclose(archivo);
        lista->eliminar(particion->id);
    }
    catch (exception e)
    {
        cout << "¡Error! No se pudo desmontar la partición. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
}