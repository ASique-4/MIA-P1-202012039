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
        cout << "Desmontando partición con ID: " << particion->id << endl;
        cout << "Status: " << particion->particion->part_status << endl;
        cout << lista->primero->id << endl;
        lista->eliminar(disco->id);
        cout << "Status: " << particion->particion->part_status << endl;
        cout << "¡Partición desmontada con éxito!" << endl;
    }
    catch (exception e)
    {
        cout << "¡Error! No se pudo desmontar la partición. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
}