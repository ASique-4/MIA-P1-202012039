#include "mkdisk.h"
#include "../../Analizador/Analizador.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctime>
#include <unistd.h>
#include <iostream>


mkdisk::mkdisk(){}

void mkdisk::make_mkdisk(mkdisk *disco){
    //instrucciones del "mkdisk"
    cout << "path: " << disco->path << endl;
    cout << "size: " << disco->size << endl;
    cout << "unit: " << disco->unit << endl;
    cout << "fit: " << disco->fit << endl;
    //Creamos el archivo
    FILE *archivo;
    string path = disco->path;

    //Verificamos si está entre comillas el path
    if(path.at(0) == '\"'){
        path = path.substr(0, path.find_last_of("/")) + "\"";
        //Quitamos comillas
        disco->path = disco->path.substr(1, disco->path.length() - 2);
    }else{
        path = path.substr(0, path.find_last_of("/"));
    }


    //Verificamos si el path existe y si no lo creamos
    if(access(path.c_str(), F_OK) == -1){
        string comando = "mkdir -p " + path;
        system(comando.c_str());
    }
    
    //Creamos el archivo
    archivo = fopen(disco->path.c_str(), "ab");
    

    //Verificamos si el archivo se creo correctamente
    if(archivo == NULL){
        cout << "!Error¡ Fallé al crear el archivo. Lo siento, parece que no soy tan hábil como pensaba." << endl;
        return;
    }
    //Llenamos el archivo con 0
    int tamanio = disco->size;

    if(disco->unit == "K"){
        tamanio = disco->size;
    } else if (disco->unit == "M"){
        tamanio = disco->size * 1024 * 1024;
    } else if (disco->unit == ""){
        tamanio = disco->size * 1024 * 1024;
    } else {
        cout << "Error en el parametro 'unit'" << endl;
        return;
    }


    for(int i = 0; i < tamanio; i++){
        fwrite("0", sizeof(char), 1, archivo);
    }

    //Guardamos el disco en el archivo
    fwrite(&disco, sizeof(mkdisk), 1, archivo);
    fclose(archivo);
    

}