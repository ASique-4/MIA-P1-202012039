#include "rmusr.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

Rmusr::Rmusr()
{
}

void Rmusr::ejecutar(string path, int start, Rmusr *rmusr)
{
    FILE *archivo;
    archivo = fopen(path.c_str(), "r+b");
    if (archivo == NULL)
    {
        cout << "No se pudo abrir el archivo" << endl;
        return;
    }

    // Comprobamos que la posición de inicio esté dentro del archivo
    fseek(archivo, 0, SEEK_END);
    int fin_archivo = ftell(archivo);
    if (start >= fin_archivo)
    {
        cout << "Posición de inicio fuera del archivo" << endl;
        fclose(archivo);
        return;
    }

    // Buscamos el grupo
    int pos = start;
    bool encontrado = false;
    char buffer[256];
    fseek(archivo, start, SEEK_SET);
    string usuario = rmusr->user;
    while (fgets(buffer, 256, archivo) != NULL) {
        string linea(buffer);
        if (linea.find(",U," + usuario) != string::npos) {
            encontrado = true;
            break;
        }
        pos = ftell(archivo);
    }

    if (!encontrado) {
        cout << "No se encontró el usuario con id " << usuario << endl;
        fclose(archivo);
        return;
    }


    // Reemplazamos el número del usuario 0,U,usuario,grupo,contraseña
    fseek(archivo, pos, SEEK_SET);
    char buffer2[256];
    string linea;
    fgets(buffer2, 256, archivo);
    linea = buffer2;
    linea.replace(0, 1, "0");
    fseek(archivo, pos, SEEK_SET);
    fputs(linea.c_str(), archivo);
    


    fclose(archivo);
    cout << "Se eliminó el usuario con id: " << usuario << endl;
}


