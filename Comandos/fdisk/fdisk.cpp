/**
 * It creates a partition
 */
#include "fdisk.h"
#include "../mkdisk/mkdisk.h"
#include <iostream>
#include <string.h>

using namespace std;

fdisk::fdisk() {}

// Función para agregar espacio a una partición
void agregar_espacio(fdisk *particion_comando){
    
}


/**
 * Toma un comando y una partición, e intenta encontrar la mejor opción para la partición en el disco.
 * 
 * @param particion_comando La estructura fdisk que contiene la ruta al disco y el nombre de la
 * partición.
 * @param particion La partición que se agregará al disco.
 */
void mejor_ajuste(fdisk *particion_comando, Partition particion){

    // Abrimos el mbr
    FILE *archivo;
    archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
    // Leemos el mbr
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Guardamos el tamaño de la nueva partición
    int tamano = particion.part_size;
    // Guardamos los espacios libres en un array de estructuras
    struct EspacioLibre {
        int size;
        Partition* partition;
    };
    EspacioLibre espacios_libres[4] = {
        {mbr.mbr_partition_1.part_size, &mbr.mbr_partition_1},
        {mbr.mbr_partition_2.part_size, &mbr.mbr_partition_2},
        {mbr.mbr_partition_3.part_size, &mbr.mbr_partition_3},
        {mbr.mbr_partition_4.part_size, &mbr.mbr_partition_4}
    };
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].partition->part_name[0] != '\0'){
            espacios_libres[i].size = -1;
        }
    }

    // Comparamos los espacios libres el más cercano al tamaño de la partición nueva
    int mejor_ajuste = -1;
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].size >= tamano && espacios_libres[i].size != -1){
            if (mejor_ajuste == -1){
                mejor_ajuste = espacios_libres[i].size;
            } else if (espacios_libres[i].size < mejor_ajuste){
                mejor_ajuste = espacios_libres[i].size;
            }
        }
    }

    // Si encontramos el mejor ajuste, asignamos la partición
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].size == mejor_ajuste && espacios_libres[i].partition->part_name[0] == '\0'){
            *(espacios_libres[i].partition) = particion;
            fseek(archivo, 0, SEEK_SET);
            fwrite(&mbr, sizeof(MBR), 1, archivo);
            fclose(archivo);
            cout << "¡Partición creada con éxito!" << endl;
            return;
        }
    }
    

    // Si no encontramos un espacio libre adecuado, mostramos un mensaje de error
    cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
    fclose(archivo);
}

/**
 * Encuentra el mayor espacio libre en el disco y le asigna la partición
 * 
 * @param particion_comando La estructura fdisk que contiene la ruta al disco y el nombre de la
 * partición.
 * @param particion La partición a crear.
 * 
 */
void peor_ajuste(fdisk *particion_comando, Partition particion){

    // Abrimos el mbr
    FILE *archivo;
    archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
    // Leemos el mbr
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Guardamos el tamaño de la nueva partición
    int tamano = particion.part_size;
    // Guardamos los espacios libres en un array de estructuras
    struct EspacioLibre {
        int size;
        Partition* partition;
    };
    EspacioLibre espacios_libres[4] = {
        {mbr.mbr_partition_1.part_size, &mbr.mbr_partition_1},
        {mbr.mbr_partition_2.part_size, &mbr.mbr_partition_2},
        {mbr.mbr_partition_3.part_size, &mbr.mbr_partition_3},
        {mbr.mbr_partition_4.part_size, &mbr.mbr_partition_4}
    };
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].partition->part_name[0] != '\0'){
            espacios_libres[i].size = -1;
        }
    }

    // Buscamos el espacio libre más grande
    int peor_ajuste = -1;
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].size >= tamano && espacios_libres[i].size != -1){
            if (peor_ajuste == -1){
                peor_ajuste = espacios_libres[i].size;
            } else if (espacios_libres[i].size > peor_ajuste){
                peor_ajuste = espacios_libres[i].size;
            }
        }
    }

    // Si encontramos el mejor ajuste, asignamos la partición
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].size == peor_ajuste && espacios_libres[i].partition->part_name[0] == '\0'){
            *(espacios_libres[i].partition) = particion;
            fseek(archivo, 0, SEEK_SET);
            fwrite(&mbr, sizeof(MBR), 1, archivo);
            fclose(archivo);
            cout << "¡Partición creada con éxito!" << endl;
            return;
        }
    }

    // Si no encontramos un espacio libre adecuado, mostramos un mensaje de error
    cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
    fclose(archivo);
}


/**
 * Encuentra el primer espacio libre en el disco y le asigna la partición
 * 
 * @param particion_comando La estructura fdisk que contiene la ruta al disco y el nombre de la
 * partición.
 * @param particion La partición a crear.
 * 
 */
void primer_ajuste(fdisk *particion_comando, Partition particion){

    // Abrimos el mbr
    FILE *archivo;
    archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
    // Leemos el mbr
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Guardamos el tamaño de la nueva partición
    int tamano = particion.part_size;
    // Guardamos los espacios libres en un array de estructuras
    struct EspacioLibre {
        int size;
        Partition* partition;
    };
    EspacioLibre espacios_libres[4] = {
        {mbr.mbr_partition_1.part_size, &mbr.mbr_partition_1},
        {mbr.mbr_partition_2.part_size, &mbr.mbr_partition_2},
        {mbr.mbr_partition_3.part_size, &mbr.mbr_partition_3},
        {mbr.mbr_partition_4.part_size, &mbr.mbr_partition_4}
    };
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].partition->part_name[0] != '\0'){
            espacios_libres[i].size = -1;
        }
    }

    // Buscamos el primer espacio libre
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i].size >= tamano && espacios_libres[i].size != -1){
            *(espacios_libres[i].partition) = particion;
            fseek(archivo, 0, SEEK_SET);
            fwrite(&mbr, sizeof(MBR), 1, archivo);
            fclose(archivo);
            cout << "¡Partición creada con éxito!" << endl;
            return;
        }
    }

    // Si no encontramos un espacio libre adecuado, mostramos un mensaje de error
    cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
    fclose(archivo);    
}
    


// Función para crear una partición
/**
 * It creates a partition
 * 
 * @param particion The partition to be created.
 * @param particion_comando The command that the user entered.
 * 
 */
void crear_particion(Partition particion, fdisk *particion_comando){
    FILE *archivo;
    archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }

    // Leemos el mbr
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);
    fclose(archivo);

    // Verificamos que la partición no exista
    for (int i = 0; i < 4; i++) {
        if (strcmp(mbr.getPartition(i).part_name, particion.part_name) == 0) {
            cout << "¡Error! Ya hay una partición con ese nombre. No seas poco creativo y ponle otro." << endl;
            return;
        }
    }

    // Si es una partición extendida
    if (particion.part_type == 'e' || particion.part_type == 'E') {
        // Verificamos que no exista una partición extendida
        for (int i = 0; i < 4; i++) {
            if (mbr.getPartition(i).part_type == 'e' || mbr.getPartition(i).part_type == 'E') {
                cout << "¡Error! Ya existe una partición extendida en este disco. Imposible crear otra." << endl;
                return;
            }
        }
    }

    // Si es una partición lógica
    if (particion.part_type == 'l' || particion.part_type == 'L'){
        // Verificamos que exista una partición extendida
        bool has_extended_partition = false;
        for (int i = 0; i < 4; i++) {
            if (mbr.getPartition(i).part_type == 'e' || mbr.getPartition(i).part_type == 'E') {
                has_extended_partition = true;
                break;
            }
        }
        if (!has_extended_partition){
            cout << "¡Error! La partición Lógica se siente sola sin su amiga Extendida" << endl;
            return;
        }
    }

    // Si es una partición primaria
    if (particion.part_type == 'p' || particion.part_type == 'P' || (particion.part_type != 'e' && particion.part_type != 'E' && particion.part_type != 'l' && particion.part_type != 'L')){
        cout << "Primaria" << endl;
        mejor_ajuste(particion_comando, particion);
    }
}



bool confirmar_eliminación(){
    char respuesta;
    cout << "Eliminar esta partición no tendrá marcha atrás. ¿Estás preparado? (s/n): ";
    cin >> respuesta;
    return (respuesta == 's' || respuesta == 'S');
}


/**
 * Elimina la partición.
 * 
 * @param partition La partición a eliminar.
 */
void delete_partition(fdisk *partition){
    // Abrimos el mbr
    FILE *archivo;
    if ((archivo = fopen(partition->path.c_str(), "rb+")) == NULL)
    {
        cout << "¡Error! No se ha encontrado el archivo." << endl;
        return;
    }

    // Leemos el mbr
    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Buscamos la partición y la eliminamos si existe
    Partition* particion = mbr.findPartition(partition->name);
    if (particion != NULL && confirmar_eliminación()){
        *particion = {};
    } else {
        cout << "¡Error! No se ha encontrado la partición." << endl;
        fclose(archivo);
        return;
    }

    // Escribimos el mbr
    fseek(archivo, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, archivo);
    fclose(archivo);
}


/**
 * Es el main del fdisk donde se analiza el comando ingresado por el usuario.
 * 
 * @param partition_comando Este es el objeto que contiene todos los parámetros que el usuario ingresó
 * en la línea de comando.
 */
void fdisk::make_fdisk(fdisk *partition_comando)
{
    // instrucciones del "fdisk"
    cout << "size: " << partition_comando->size << endl;
    cout << "path: " << partition_comando->path << endl;
    cout << "name: " << partition_comando->name << endl;
    cout << "unit: " << partition_comando->unit << endl;
    cout << "type: " << partition_comando->type << endl;
    cout << "fit: " << partition_comando->fit << endl;
    cout << "delete: " << partition_comando->delete_ << endl;
    cout << "add: " << partition_comando->add << endl;

    // Verificamos si el archivo existe
    try
    {

        FILE *archivo;
        archivo = fopen(partition_comando->path.c_str(), "ab");
        if (archivo == NULL)
        {
            throw 1;
        }
        fclose(archivo);
    }
    catch (int e)
    {
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }

    cout << "delete: " << partition_comando->delete_ << "." << endl;

    // Verificamos qué se va a hacer con la partición
    if (partition_comando->delete_ == "full" || partition_comando->delete_ == "FULL"){
        delete_partition(partition_comando);
    } else if (isdigit(partition_comando->add)){
        agregar_espacio(partition_comando);
    } else if (partition_comando->delete_ == "" && !isdigit(partition_comando->add)){
        Partition new_particion;
        strcpy(new_particion.part_name, partition_comando->name);
        new_particion.part_size = partition_comando->size;
        new_particion.part_fit = partition_comando->fit;
        new_particion.part_type = partition_comando->type;
        new_particion.part_status = '0';
        crear_particion(new_particion,partition_comando);
    } else {
        cout << "¡Error! No tengo ni idea de lo que intentas hacer con esa partición. ¿Podrías darme una pista?" << endl;
        return;
    }

    // Leemos el mbr
    FILE *archivo;
    archivo = fopen(partition_comando->path.c_str(), "rb+");
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);
    cout << "=====Partición 1=====" << endl;
    cout << "Nombre: " << mbr.mbr_partition_1.part_name << endl;
    cout << "Tamaño: " << mbr.mbr_partition_1.part_size << endl;
    cout << "Tipo: " << mbr.mbr_partition_1.part_type << endl;
    cout << "Ajuste: " << mbr.mbr_partition_1.part_fit << endl;
    cout << "Status: " << mbr.mbr_partition_1.part_status << endl;
    cout << "=====Partición 2=====" << endl;
    cout << "Nombre: " << mbr.mbr_partition_2.part_name << endl;
    cout << "Tamaño: " << mbr.mbr_partition_2.part_size << endl;
    cout << "Tipo: " << mbr.mbr_partition_2.part_type << endl;
    cout << "Ajuste: " << mbr.mbr_partition_2.part_fit << endl;
    cout << "Status: " << mbr.mbr_partition_2.part_status << endl;
    cout << "=====Partición 3=====" << endl;
    cout << "Nombre: " << mbr.mbr_partition_3.part_name << endl;
    cout << "Tamaño: " << mbr.mbr_partition_3.part_size << endl;
    cout << "Tipo: " << mbr.mbr_partition_3.part_type << endl;
    cout << "Ajuste: " << mbr.mbr_partition_3.part_fit << endl;
    cout << "Status: " << mbr.mbr_partition_3.part_status << endl;
    cout << "=====Partición 4=====" << endl;
    cout << "Nombre: " << mbr.mbr_partition_4.part_name << endl;
    cout << "Tamaño: " << mbr.mbr_partition_4.part_size << endl;
    cout << "Tipo: " << mbr.mbr_partition_4.part_type << endl;
    cout << "Ajuste: " << mbr.mbr_partition_4.part_fit << endl;
    cout << "Status: " << mbr.mbr_partition_4.part_status << endl;


    fclose(archivo);

    

}