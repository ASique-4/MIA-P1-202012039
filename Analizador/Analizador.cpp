#include "Analizador.h"

#include "../Comandos/rmdisk/rmdisk.h"
#include "../Comandos/mkdisk/mkdisk.h"
#include "../Comandos/fdisk/fdisk.h"
#include "../Comandos/mount/mount.h"
#include "../Comandos/unmount/unmount.h"
#include "../Comandos/mount/ListaDobleMount.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

// Variables globales
ListaDobleMount* listaMountGlobal = new ListaDobleMount();


/*Funciones que devuelven el tipo y el valor de un parametro en strings ya en lowercase */
/**
 * Toma una cadena y devuelve el tipo del parámetro.
 * 
 * @param parametro El parámetro a analizar.
 * 
 * @return El tipo de parámetro
 */
string get_tipo_parametro(string parametro){
    //Iteramos hasta obtener el tipo del parametro
    string tipo = "";
    for(int i = 0; i < parametro.length(); i++){
        if(parametro.at(i) == '=') break;
        char caracter = tolower(parametro.at(i));
        tipo = tipo + caracter;
    }
    //devolvemos el string
    return tipo.erase(tipo.find_last_not_of(" \t\r\n") + 1);
}
/**
 * Toma una cadena y devuelve el valor del parámetro.
 * 
 * @param parametro El parámetro a analizar.
 * 
 * @return El valor del parámetro.
 */
string get_valor_parametro(string parametro){
    //Iteramos hasta obtener el tipo del parametro
    string valor = "";
    bool concatenar = false;
    for(int i = 0; i < parametro.length(); i++){
        if(concatenar){
            char caracter = parametro.at(i);
            valor = valor + caracter;
        }
        if(parametro.at(i) == '=') concatenar = true;
    }
    //devolvemos el string
    return valor.erase(valor.find_last_not_of(" \t\r\n") + 1);
}


/**
 * Toma una cadena, la divide en tokens y luego asigna los tokens a las variables apropiadas
 * 
 * @param parametros La cadena que contiene los parámetros del comando.
 * 
 * @return Un puntero a un objeto mkdisk
 */
void analizar_mkdisk(char *parametros){
    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, ">");
    //Inicializamos nuestro disco
    mkdisk *disco = new mkdisk();
    while(parametros != NULL){
        //Obtenemos el tipo y el valor del parametro actual
        string tmpParam = parametros;
        string tipo = get_tipo_parametro(tmpParam);
        string valor = get_valor_parametro(tmpParam);
        //Verificamos cual parametro es para inicializar el objeto (los parametros ya vienen en lowercase)
        if(tipo == "size"){
            disco->size = stoi(valor);
        } else if (tipo == "path"){
            disco->path = valor.erase(valor.find_last_not_of(" \t\r\n") + 1);
        } else if (tipo == "unit"){
            disco->unit = valor[0];
        } else if (tipo == "fit"){
            disco->fit = valor[0];
        } else {
            cout << "¡Error! mkdisk solo acepta parámetros válidos, ¿qué intentas hacer con '" << valor << "'?" << endl;
        }
        parametros = strtok(NULL, ">");
    }
    //Verificamos que los parametros obligatorios esten
    if (disco->size <= 0 || disco->path == ""){
        cout << "¡Error! Parece que alguien olvidó poner los parámetros en 'mkdisk'" << endl;
        return;
    }
    //Creamos el disco
    disco->make_mkdisk(disco);
}

/**
 * Toma una cadena, la divide en tokens y luego usa esos tokens para crear un nuevo objeto rmdisk
 * 
 * @param parametros La cadena que contiene los parámetros.
 * 
 * @return un puntero a un char.
 */
void analizar_rmdisk(char *parametros){
    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, ">");
    //Inicializamos nuestro disco
    rmdisk *disco = new rmdisk();
    while(parametros != NULL){
        //Obtenemos el tipo y el valor del parametro actual
        string tmpParam = parametros;
        string tipo = get_tipo_parametro(tmpParam);
        string valor = get_valor_parametro(tmpParam);
        //Verificamos cual parametro es para inicializar el objeto (los parametros ya vienen en lowercase)
        if(tipo == "path"){
            disco->path = valor.erase(valor.find_last_not_of(" \t\r\n") + 1);
        } else {
            cout << "¡Error! rmdisk solo acepta parámetros válidos, ¿qué intentas hacer con '" << valor << "'?" << endl;
        }
        parametros = strtok(NULL, ">");
    }
    //Verificamos que los parametros obligatorios esten
    if (disco->path == ""){
        cout << "¡Error! Parece que alguien olvidó poner los parámetros en 'rmdisk'" << endl;
        return;
    }
    //Creamos el disco
    disco->make_rmdisk(disco);
}


/**
 * Toma los parámetros del comando y crea un nuevo objeto fdisk con ellos.
 * 
 * @param parametros Los parámetros que el usuario ingresó.
 * 
 * @return un puntero a un objeto fdisk.
 */
void analizar_fdisk(char *parametros){
    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, ">");
    //Inicializamos nuestro disco
    fdisk *particion = new fdisk();
    while(parametros != NULL){
        //Obtenemos el tipo y el valor del parametro actual
        string tmpParam = parametros;
        string tipo = get_tipo_parametro(tmpParam);
        string valor = get_valor_parametro(tmpParam);
        //Verificamos cual parametro es para inicializar el objeto (los parametros ya vienen en lowercase)
        if(tipo == "size"){
            particion->size = stoi(valor);
        } else if (tipo == "path"){
            particion->path = valor.erase(valor.find_last_not_of(" \t\r\n") + 1);
        } else if (tipo == "unit"){
            particion->unit = valor[0];
        } else if (tipo == "fit"){
            particion->fit = valor[0];
        } else if (tipo == "type"){
            particion->type = valor[0];
        } else if (tipo == "delete"){
            particion->delete_ = valor;
        } else if (tipo == "name"){
            strcpy(particion->name, valor.c_str());
        } else if (tipo == "add"){
            particion->add = stoi(valor);
        } else {
            cout << "¡Error! fdisk solo acepta parámetros válidos, ¿qué intentas hacer con '" << valor << "'?" << endl;
        }
        parametros = strtok(NULL, ">");
    }
    //Verificamos que los parametros obligatorios esten
    if (particion->path == "" || particion->name == ""){
        cout << "¡Error! Parece que alguien olvidó poner los parámetros en 'fdisk'" << endl;
        return;
    }
    //Creamos la particion
    particion->make_fdisk(particion);
}


/**
 * Toma dos listas de tipo ListaDobleMount y agrega la segunda lista a la primera
 * 
 * @param lista1 La lista donde se agregarán los datos.
 * @param lista2 La lista que contiene los datos que se añadirán a lista1
 */
void agregarMount(ListaDobleMount* lista1, ListaDobleMount* lista2)
{
    ParticionMount* aux = lista2->primero;
    while(aux != NULL){
        lista1->insertar(aux);
        aux = aux->siguiente;
    }
}

/**
 * Toma una cadena, la divide en tokens y luego usa esos tokens para crear un objeto MOUNT
 * 
 * @param parametros Los parámetros que el usuario ingresó.
 * 
 */
void analizar_mount(char *parametros){
    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, ">");
    //Inicializamos nuestro disco
    MOUNT *particion = new MOUNT();
    while(parametros != NULL){
        //Obtenemos el tipo y el valor del parametro actual
        string tmpParam = parametros;
        string tipo = get_tipo_parametro(tmpParam);
        string valor = get_valor_parametro(tmpParam);
        //Verificamos cual parametro es para inicializar el objeto (los parametros ya vienen en lowercase)
        if(tipo == "path"){
            particion->path = valor.erase(valor.find_last_not_of(" \t\r\n") + 1);
        } else if (tipo == "name"){
            strcpy(particion->name, valor.c_str());
        } else {
            cout << "¡Error! mount solo acepta parámetros válidos, ¿qué intentas hacer con '" << valor << "'?" << endl;
        }
        parametros = strtok(NULL, ">");
    }
    //Verificamos que los parametros obligatorios esten
    if (particion->path == "" || particion->name == ""){
        cout << "¡Error! Parece que alguien olvidó poner los parámetros en 'mount'" << endl;
        return;
    }
    //Creamos la particion
    agregarMount(listaMountGlobal, particion->make_mount(particion));
}

void analizar_unmount(char *parametros){
    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, ">");
    //Inicializamos nuestro disco
    UNMOUNT *particion = new UNMOUNT();
    while(parametros != NULL){
        //Obtenemos el tipo y el valor del parametro actual
        string tmpParam = parametros;
        string tipo = get_tipo_parametro(tmpParam);
        string valor = get_valor_parametro(tmpParam);
        //Verificamos cual parametro es para inicializar el objeto (los parametros ya vienen en lowercase)
        if(tipo == "id"){
            particion->id = valor;
        } else {
            cout << "¡Error! unmount solo acepta parámetros válidos, ¿qué intentas hacer con '" << valor << "'?" << endl;
        }
        parametros = strtok(NULL, ">");
    }
    //Verificamos que los parametros obligatorios esten
    if (particion->id == ""){
        cout << "¡Error! Parece que alguien olvidó poner los parámetros en 'unmount'" << endl;
        return;
    }
    //Creamos la particion
    particion->make_unmount(particion, listaMountGlobal);
}

/**
 * Toma una cadena, la divide en tokens y luego llama a la función apropiada para manejar el comando
 * 
 * @param comando El comando que el usuario ingresó.
 */
void Analizar(char* comando)
{
    char* token = strtok(comando, " ");
    if(strcasecmp(token, "mkdisk") == 0){
        cout << "mkdisk" << endl;
        analizar_mkdisk(token);
    }else if(strcasecmp(token, "rmdisk") == 0){
        cout << "rmdisk" << endl;
        analizar_rmdisk(token);
    }else if(strcasecmp(token, "fdisk") == 0){
        cout << "fdisk" << endl;
        analizar_fdisk(token);
    }else if(strcasecmp(token, "mount") == 0){
        cout << "mount" << endl;
        analizar_mount(token);
    }else if(strcasecmp(token, "unmount") == 0){
        cout << "unmount" << endl;
        analizar_unmount(token);
    }else if(strcasecmp(token, "exit") == 0){
        cout << "exit" << endl;
        exit(0);
    }else{
        cout << "¡Error! No puedo hacer eso, ¡soy un programa, no un mago!" << endl;
    }

}
