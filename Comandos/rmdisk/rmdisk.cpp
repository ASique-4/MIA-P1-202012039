#include "rmdisk.h"
#include <iostream>
#include <string.h>
#include <ctime>
#include <unistd.h>

rmdisk::rmdisk(){}

/**
 * Elimina un archivo.
 * 
 * @param disco Este es el objeto que contiene la información del disco a eliminar.
 * 
 * @return Nada
 */
void rmdisk::make_rmdisk(rmdisk *disco){
    //instrucciones del "rmdisk"
    cout << "path: " << disco->path << endl;
    //Verificamos si el archivo existe
    try{
        //Eliminamos el archivo
        string comando = "rm " + disco->path;
        system(comando.c_str());
    }catch(exception e){
        cout << "¡Error No se pudo eliminar el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
}

