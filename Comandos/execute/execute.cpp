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
        // Eliminamos el salto de linea
        linea[strlen(linea) - 1] = '\0';
        // Ejecutamos la linea
        Analizar(linea);
    }
    fclose(archivo);
}