#include "ListaDobleMount.h"

#include <iostream>
#include <string.h>

using namespace std;

void ListaDobleMount::insertar(ParticionMount *particion)
{
    if (primero == NULL)
    {
        primero = particion;
        ultimo = particion;
    }
    else
    {
        ultimo->siguiente = particion;
        particion->anterior = ultimo;
        ultimo = particion;
    }
}

void ListaDobleMount::imprimir()
{
    ParticionMount *aux = primero;
    cout << "---------------------------------" << endl;
    while (aux != NULL)
    {
        cout << "ID: " << aux->id << endl;
        cout << "---------------------------------" << endl;
        aux = aux->siguiente;
    }
}


ParticionMount *ListaDobleMount::buscar(string id)
{
    ParticionMount *aux = primero;
    while (aux != NULL)
    {
        if (aux->id == id)
        {
            return aux;
        }
        aux = aux->siguiente;
    }
    return NULL;
}

void ListaDobleMount::eliminar(string id)
{
    ParticionMount *aux = buscar(id);
    if (aux != NULL)
    {
        cout << "Encontrado" << endl;
        if (aux == primero)
        {
            primero = primero->siguiente;
            if (primero != NULL)
            {
                primero->anterior = NULL;
            }
        }
        else if (aux == ultimo)
        {
            ultimo = ultimo->anterior;
            if (ultimo != NULL)
            {
                ultimo->siguiente = NULL;
            }
        }
        else
        {
            aux->anterior->siguiente = aux->siguiente;
            aux->siguiente->anterior = aux->anterior;
        }
        delete aux;
    }
}
