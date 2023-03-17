#include "login.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>

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
    string pathCarpeta = listaDobleMount->buscar(login->id)->path;
    
    // Quitamos el archivo de la ruta
    pathCarpeta = pathCarpeta.substr(0, pathCarpeta.find_last_of("/"));
    // Verificar si existe el archivo de usuarios
    string pathArchivo = pathCarpeta + "/usuarios.txt";
    archivo = fopen(pathArchivo.c_str(), "r");
    if (archivo == NULL)
    {
        cout << "Path: " << pathArchivo << endl;
        cout << "Error: No se pudo abrir el archivo de usuarios" << endl;
        return nullptr;
    }
    // Verificar si existe el usuario
    User* user;
    char linea[100];
    while (fgets(linea, 100, archivo) != NULL)
    {
        try
        {
            string lineaString = linea;
            // id
            user->id = lineaString.substr(0, lineaString.find_first_of(","));
            // tipo 
            lineaString = lineaString.substr(lineaString.find_first_of(",") + 1);
            user->tipo = lineaString.substr(0, lineaString.find_first_of(","));
            // grupo
            lineaString = lineaString.substr(lineaString.find_first_of(",") + 1);
            user->grupo = lineaString.substr(0, lineaString.find_first_of(","));

            if (user->tipo == "U")
            {    
                // usuario
                lineaString = lineaString.substr(lineaString.find_first_of(",") + 1);
                user->usuario = lineaString.substr(0, lineaString.find_first_of(","));
                // password
                lineaString = lineaString.substr(lineaString.find_first_of(",") + 1);
                user->password = lineaString.substr(0, lineaString.find_first_of(","));

            }
        } catch (const std::exception& e)
        {
            user = new User();
            cout << "Error: Archivo de usuarios corrupto" << endl;
            return nullptr;
        }
    
        if (user->usuario == login->user)
        {
            if (password == login->password)
            {
                cout << "Login exitoso" << endl;
                return user;
            }
            else
            {
                cout << "Error: Contraseña incorrecta" << endl;
                return nullptr;
            }
        }
    }
    cout << "Error: Usuario no encontrado" << endl;
    return nullptr;

    
}