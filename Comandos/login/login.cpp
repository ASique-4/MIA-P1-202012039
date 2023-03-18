#include "login.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "../fdisk/partition.h"
#include "../Estructuras/BloqueDeCarpetas.h"

using namespace std;

Login::Login()
{
    user = "";
    password = "";
    id = "";
}


User* Login::make_login(Login *login, ListaDobleMount *listaDobleMount)
{
    
    FILE *archivo;
    string pathArchivo = listaDobleMount->buscar(login->id)->path;
    archivo = fopen(pathArchivo.c_str(), "r");
    if (archivo == NULL)
    {
        cout << "Error: No se pudo abrir el archivo de usuarios" << endl;
        return nullptr;
    }
    // Vamos a la posicion del archivo donde se encuentran los usuarios
    fseek(archivo, 0, SEEK_SET);
    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, archivo);
    Partition *particiones[4] = {&mbr.mbr_partition_1, &mbr.mbr_partition_2, &mbr.mbr_partition_3, &mbr.mbr_partition_4};
    // Buscamos la particion que contiene el archivo de usuarios
    Partition *particion = nullptr;
    for (int i = 0; i < 4; i++)
    {
        if (particiones[i]->part_status == '1' && strcmp(particiones[i]->part_name, listaDobleMount->buscar(login->id)->name) == 0)
        {
            particion = particiones[i];
            break;
        }
    }
    // Abrimos el SuperBloque
    SuperBloque *superBloque = new SuperBloque();
    fseek(archivo, particion->part_start, SEEK_SET);
    fread(superBloque, sizeof(SuperBloque), 1, archivo);
    // Nos posicionamos en el inicio del archivo de usuarios
    fseek(archivo,superBloque->s_block_start + sizeof(BloqueDeCarpetas), SEEK_SET);
    // Leemos el archivo de usuarios
    User* user;
    char linea[100];
    
    while (fgets(linea, 100, archivo) != NULL)
    {
        user = new User();
        user->txtPos = superBloque->s_block_start + sizeof(BloqueDeCarpetas);
        user->pathUsuarios = pathArchivo;
        
        char *token = strtok(linea, ",");
        user->id = token;
        token = strtok(NULL, ",");
        user->tipo = token;
        try
        {
            token = strtok(NULL, ",");
            user->grupo = token;
        }catch(exception e)
        {
            token = strtok(NULL, "\n");
            user->grupo = token;
        }

        if (user->tipo == "U")
        {
            token = strtok(NULL, ",");
            user->usuario = token;
            token = strtok(NULL, ",");
            user->password = token;
            // Eliminar el salto de linea
            user->password = user->password.substr(0, user->password.size() - 1);
            if (user->usuario == login->user && user->password == login->password)
            {
                cout << "Se ha iniciado sesion" << endl;
                fclose(archivo);
                return user;
            }
        }
        
    }
    cout << "Error: Usuario o contraseña incorrectos" << endl;
    fclose(archivo);
    return nullptr;

    
}