#include "mount.h"
#include <iostream>
#include <string.h>
#include "../fdisk/ebr.h"
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
    int num2 = -1;
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
        // Verificamos si es particion logica
        for (int i = 0; i < 4; i++)
        {
            if (mbr.getPartition(i).part_type == 'e')
            {
                num2 = i;
                EBR ebr;
                fseek(archivo, mbr.getPartition(i).part_start, SEEK_SET);
                fread(&ebr, sizeof(EBR), 1, archivo);
                while (ebr.part_next != -1)
                {
                    if (strcmp(ebr.part_name, disco->name) == 0)
                    {
                        num = i;
                        break;
                    }
                    fseek(archivo, ebr.part_next, SEEK_SET);
                    fread(&ebr, sizeof(EBR), 1, archivo);
                }
            }
        }
        if (num == -1)
        {
            cout << "No se encontró la partición" << endl;
            return "";
        }
    }
    // Nombre del archivo
    string name = disco->path;
    // Eliminamos la extensión
    name = name.substr(0, name.find_last_of("."));
    // Eliminamos el path
    name = name.substr(name.find_last_of("/") + 1, name.length());
    // Creamos el ID
    if(num2 != -1)
    {
        return "39" + to_string(num) + to_string(num2) + name;
    }
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
    strcpy(particion->name, disco->name);
    particion->path = disco->path;
    try
    {
        bool found = false;
        // Buscamos la partición
        for (int i = 0; i < 4; i++)
        {
            if (strcmp(mbr.getPartition(i).part_name, disco->name) == 0)
            {
                mbr.findPartition(mbr.getPartition(i).part_name)->part_status = '1';
                found = true;
                break;
            }
        }
        // Si no se encontró la partición
        if (!found)
        {
            // Verificamos si es particion logica
            for (int i = 0; i < 4; i++)
            {
                if (mbr.getPartition(i).part_type == 'e')
                {
                    EBR ebr;
                    fseek(archivo, mbr.getPartition(i).part_start, SEEK_SET);
                    fread(&ebr, sizeof(EBR), 1, archivo);
                    while (ebr.part_next != -1)
                    {
                        if (strcmp(ebr.part_name, disco->name) == 0)
                        {
                            ebr.part_status = '1';
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
                return nullptr;
            }
        }
    }
    catch (const std::exception& e)
    {
        cout << "No se encontró la partición" << endl;
        return nullptr;
    }
    // Agregamos la partición a la lista
    ListaDobleMount* lista = new ListaDobleMount();
    particion->montada = true;
    // Hora de montaje
    particion->mount_time = time(0);
    // Aumentamos el contador de montajes
    particion->mnt_count++;
    // Agregamos la partición a la lista
    lista->insertar(particion);
    // Escribimos el mbr
    fseek(archivo, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, archivo);
    // Cerramos el archivo
    fclose(archivo);
    // Retornamos la lista
    return lista;
}