#ifndef LISTADOBLEMOUNT_H
#define LISTADOBLEMOUNT_H

#include <iostream>
#include "ParticionMount.h"
#include <string.h>

using namespace std;

class ListaDobleMount
{
    public:
        ListaDobleMount();
        ParticionMount *primero = NULL;
        ParticionMount *ultimo = NULL;
        void insertar(ParticionMount *particion);
        void imprimir();
        ParticionMount *buscar(string id);
        void eliminar(string id);
    private:
    
};

#endif // LISTADOBLEMOUNT_H