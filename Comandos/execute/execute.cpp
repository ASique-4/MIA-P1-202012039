#include "execute.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

Execute::Execute()
{
}


void Execute::ejecutar(Execute* ex)
{
    FILE *archivo;
    archivo = fopen(ex->path.c_str(), "r");
    if (archivo == NULL)
    {
        cout << "Error: No se pudo abrir el disco" << endl;
        return;
    }
    // Ejecutamos cada linea del archivo
    char linea[100];
    while (fgets(linea, 100, archivo) != NULL)
    {
        // Eliminamos el salto de linea a menos que sea el ultimo caracter
        if (linea[strlen(linea) - 1] == '\n')
        {
            linea[strlen(linea) - 1] = '\0';
        }
        // Si viene un comentario
        if (linea[0] == '#')
        {
            continue;
        }
        // Ejecutamos la linea
        Analizar(linea);
    }
    fclose(archivo);
}