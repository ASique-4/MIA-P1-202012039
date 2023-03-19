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

MBR::MBR(){}

void mkdisk::make_mkdisk(mkdisk *disco){
    
    //Creamos el archivo
    FILE *archivo;
    string path = disco->path;

    //Verificamos si está entre comillas el path
    std::string comillas("\"");
    if (path.length() >= 2 && path.substr(path.length() - 2) == comillas) {
        disco->path = path.substr(0, path.length() - 2);
    }


    //Verificamos si el path existe y si no lo creamos
    if(access(path.c_str(), F_OK) == -1){
        string comando = "mkdir -p " + disco->path.substr(0, path.find_last_of("/"));
        system(comando.c_str());
    }
    
    //Creamos el archivo
    archivo = fopen(disco->path.c_str(), "ab");
    

    //Verificamos si el archivo se creo correctamente
    if(archivo == NULL){
        cout << "¡Error Fallé al crear el archivo. Lo siento, parece que no soy tan hábil como pensaba." << endl;
        return;
    }
    //Llenamos el archivo con 0
    int tamanio = disco->size;

    //Verificamos si el tamaño es en KB
    if(disco->unit == 'K'){
        tamanio = tamanio * 1024;
    }else if(disco->unit == 'M'){
        tamanio = tamanio * 1024 * 1024;
    }else{
        tamanio = tamanio * 1024 * 1024;
    }

    //Creamos el mbr
    disco->mbr.mbr_tamano = tamanio;
    disco->mbr.mbr_fecha_creacion = time(0);
    disco->mbr.mbr_disk_signature = rand() % 1000;
    disco->mbr.mbr_disk_fit = disco->fit;
    

    //Escribimos el mbr
    fwrite(&disco->mbr, sizeof(MBR), 1, archivo);
    //Llenamos el archivo con 0
    char c = 0;
    for(int i = 0; i < tamanio - sizeof(MBR); i++){
        fwrite(&c, sizeof(char), 1, archivo);
    }
    fclose(archivo);
    cout << "¡Presto! Disco creado correctamente. " << endl;

}