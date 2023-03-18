#include "mkgrp.h"
#include <iostream>
#include <string.h>

using namespace std;

MKGRP::MKGRP()
{
}
void MKGRP::ejecutarMKGRP(string id, string path, int start)
{
    FILE *archivo;
    archivo = fopen(path.c_str(), "r+b");
    if (archivo == NULL)
    {
        cout << "No se pudo abrir el archivo" << endl;
        return;
    }

    // Buscamos el último grupo
    int ultimo_grupo = -1;  // Inicializamos en -1 para detectar si no se encontró ningún grupo
    char linea[100];
    int pos = 0;
    fseek(archivo, start, SEEK_SET);
    while (fgets(linea, 100, archivo) != NULL)
    {
        string linea_str(linea);
        if (linea_str.find(",G,") != string::npos)
        {
            char* token = strtok(linea, ",");
            int numero_grupo = atoi(token);
            if (numero_grupo > ultimo_grupo)
            {
                ultimo_grupo = numero_grupo;
            }
            pos = ftell(archivo);
        }
        if (linea_str.find(",U,") != string::npos)
        {
            pos = ftell(archivo);
        }
        if (linea[0] == '\0')
        {
            break;
        }
    }


    // Creamos la cadena del nuevo grupo
    int nuevo_grupo = ultimo_grupo + 1;
    string grupo = to_string(nuevo_grupo) + ",G," + id + "\n";

    // Escribimos la cadena en el archivo en la posición del último grupo
    fseek(archivo, pos, SEEK_SET);
    fwrite(grupo.c_str(), sizeof(char), grupo.length(), archivo);

    fclose(archivo);
}






