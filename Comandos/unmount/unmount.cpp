#include "unmount.h"
#include "../mount/ListaDobleMount.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

UNMOUNT::UNMOUNT()
{
}

void UNMOUNT::make_unmount(UNMOUNT *disco, ListaDobleMount *lista)
{
    cout << "Desmontando partición..." << endl;
    cout << "ID: " << disco->id << endl;
    if (disco->id == "")
    {
        cout << "¡Error! No podemos desmontar la partición si no nos dicen cuál es. ¿Alguna pista? " << endl;
        return;
    }
    lista->imprimir();
    cout << "Buscando partición con ID: " << disco->id << endl;
    try
    {
        ParticionMount *particion = new ParticionMount();
        particion = lista->buscar(disco->id);
        if (particion == NULL)
        {
            cout << "¡Error! No se encontró la partición con el ID: " << disco->id << endl;
            return;
        }
        cout << "Partición encontrada" << endl;
        FILE *archivo = fopen(particion->path.c_str(), "rb+");
        MBR mbr;
        cout << "Leyendo MBR: " << particion->path.c_str() << endl;
        fseek(archivo, 0, SEEK_SET);
        fread(&mbr, sizeof(MBR), 1, archivo);
        mbr.findPartition(particion->name)->part_status = '0';
        cout << "Status: " << mbr.findPartition(particion->name)->part_status << endl;
        fseek(archivo, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        fclose(archivo);
        lista->eliminar(particion->id);
        cout << "¡Partición desmontada con éxito!" << endl;
    }
    catch (exception e)
    {
        cout << "¡Error! No se pudo desmontar la partición. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
}