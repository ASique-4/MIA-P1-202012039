#include "mount.h"
#include <iostream>
#include <string.h>
#include "ListaDobleMount.cpp"


using namespace std;

MOUNT::MOUNT()
{
}

ListaDobleMount::ListaDobleMount()
{
}



/**
 * Crea un ID para montar la partición.
 * 
 * @param disco La estructura MOUNT que contiene la información de la partición.
 * 
 * @return El ID de la partición.
 */
string createID(MOUNT *disco)
{
    // Número de partición
    int num = -1;
    // Abrimos el mbr
    FILE *archivo;
    archivo = fopen(disco->path.c_str(), "rb+");
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);
    // Buscamos la partición
    for (int i = 0; i < 4; i++)
    {
        if (strcmp(mbr.getPartition(i).part_name, disco->name) == 0)
        {
            num = i;
            break;
        }
    }
    // Si no se encontró la partición
    if (num == -1)
    {
        cout << "No se encontró la partición" << endl;
        return "";
    }
    // Nombre del archivo
    string name = disco->path;
    // Eliminamos la extensión
    name = name.substr(0, name.find_last_of("."));
    // Eliminamos el path
    name = name.substr(name.find_last_of("/") + 1, name.length());
    // Creamos el ID
    return "39" + to_string(num) + name;
}

ListaDobleMount* MOUNT::make_mount(MOUNT *disco)
{
    cout << "Path: " << disco->path << endl;
    cout << "Name: " << disco->name << endl;

    // Creamos el ID
    string id = createID(disco);
    // Si no se pudo crear el ID
    if (id == "")
    {
        cout << "No se pudo crear el ID, parece que no soy tan inteligente como creía" << endl;
        return nullptr;
    }
    // Abrimos el archivo
    FILE *archivo;
    archivo = fopen(disco->path.c_str(), "rb+");
    // Leemos el mbr
    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, archivo);
    // Creamos la particion mount
    ParticionMount* particion = new ParticionMount();
    particion->id = id;
    particion->particion = mbr.findPartition(disco->name);
    mbr.findPartition(disco->name)->part_status = '1';
    // Agregamos la partición a la lista
    ListaDobleMount* lista = new ListaDobleMount();
    lista->insertar(particion);
    // Escribimos el mbr
    fseek(archivo, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, archivo);
    // Cerramos el archivo
    fclose(archivo);
    // Retornamos la lista
    return lista;
}