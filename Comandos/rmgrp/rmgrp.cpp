#include "rmgrp.h"
#include <iostream>
#include <string>
#include <cstdio>


using namespace std;

RMGRP::RMGRP()
{
}

void RMGRP::ejecutarRMGRP(string id, string path, int start)
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
    while (fgets(buffer, 256, archivo) != NULL) {
        string linea(buffer);
        if (linea.find(",G," + id) != string::npos) {
            encontrado = true;
            break;
        }
        pos = ftell(archivo);
    }

    if (!encontrado) {
        cout << "No se encontró el grupo con id " << id << endl;
        fclose(archivo);
        return;
    }


    // Reemplazamos el número del grupo
    fseek(archivo, pos, SEEK_SET);
    string linea(buffer);
    size_t start_pos = linea.find_first_of(',');
    size_t end_pos = linea.find_first_of(',', start_pos + 1);
    string numero_grupo = linea.substr(start_pos + 1, end_pos - start_pos - 1);
    string nueva_linea = "0,G," + linea.substr(end_pos + 1);
    fseek(archivo, pos, SEEK_SET);
    fputs(nueva_linea.c_str(), archivo);


    fclose(archivo);
    cout << "Se eliminó el grupo con id " << id << endl;
}





