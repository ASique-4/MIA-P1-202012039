#include "login.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>


void Login::make_login(Login *login, ListaDobleMount *listaDobleMount)
{
    FILE *archivo;
    string pathCarpeta = listaDobleMount->buscar(login->id)->path.substr(0, listaDobleMount->buscar(login->id)->path.find_last_of("/"));
    
    // Verificar si existe el archivo de usuarios
    string pathArchivo = pathCarpeta + "/users.txt";
    archivo = fopen(pathArchivo.c_str(), "r");
    if (archivo == NULL)
    {
        cout << "Error: No se pudo abrir el archivo de usuarios" << endl;
        return;
    }
    // Verificar si existe el usuario
    char linea[100];
    while (fgets(linea, 100, archivo) != NULL)
    {
        // Eliminamos el salto de linea
        linea[strlen(linea) - 1] = '\0';
        // Verificamos si es el usuario
        string lineaString = linea;
        if (lineaString.find(login->user) != string::npos)
        {
            // Verificamos si la contraseña es correcta
            if (lineaString.find(login->password) != string::npos)
            {
                cout << "Login exitoso" << endl;
                fclose(archivo);
                return;
            }
            else
            {
                cout << "Error: Contraseña incorrecta" << endl;
                fclose(archivo);
                return;
            }
        }
    }

    
}