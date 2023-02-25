#include "fdisk.h"
#include "../mkdisk/mkdisk.h"
#include <iostream>
#include <string.h>

using namespace std;

fdisk::fdisk() {}

// Función para agregar espacio a una partición
void agregar_espacio(fdisk *particion_comando){
    
}

// Mejor ajuste
void mejor_ajuste(fdisk *particion_comando, Partition particion){

    // Abrimos el mbr
    FILE *archivo;
    archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "Prueba 1" << endl;
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
    // Leemos el mbr
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Si la primera partición está vacía
    if (mbr.mbr_partition_1.part_size == -1){
        mbr.mbr_partition_1 = particion;
        fseek(archivo, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        fclose(archivo);
        return;
    }

    // Guardamos el tamaño de la nueva partición
    int tamano = particion.part_size;
    // Guardamos los espacios libres
    int espacios_libres[4];
    espacios_libres[0] = mbr.mbr_partition_1.part_size;
    espacios_libres[1] = mbr.mbr_partition_2.part_size;
    espacios_libres[2] = mbr.mbr_partition_3.part_size;
    espacios_libres[3] = mbr.mbr_partition_4.part_size;

    // Comparamos los espacios libres el más cercano al tamaño de la partición nueva
    int mejor_ajuste = 0;
    for (int i = 0; i < 4; i++){
        if (espacios_libres[i] >= tamano){
            if (mejor_ajuste == 0){
                mejor_ajuste = espacios_libres[i];
            } else if (espacios_libres[i] < mejor_ajuste){
                mejor_ajuste = espacios_libres[i];
            }
        }
    }

    // Si no hay espacio suficiente
    if (mejor_ajuste == 0){
        cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
        return;
    }

    // Si el mejor ajuste es la primera partición
    if (mejor_ajuste == mbr.mbr_partition_1.part_size){
        mbr.mbr_partition_1 = particion;
        fseek(archivo, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        fclose(archivo);
        return;
    } else if (mejor_ajuste == mbr.mbr_partition_2.part_size){
        mbr.mbr_partition_2 = particion;
        fseek(archivo, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        fclose(archivo);
        return;
    } else if (mejor_ajuste == mbr.mbr_partition_3.part_size){
        mbr.mbr_partition_3 = particion;
        fseek(archivo, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        fclose(archivo);
        return;
    } else if (mejor_ajuste == mbr.mbr_partition_4.part_size){
        mbr.mbr_partition_4 = particion;
        fseek(archivo, 0, SEEK_SET);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        fclose(archivo);
        return;
    } else {
        cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
        return;
    }

}
    


// Función para crear una partición
void crear_particion(Partition particion, fdisk *particion_comando){
    FILE *archivo;
    archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "Prueba 2" << endl;
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }

    // Leemos el mbr
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Verificamos que la partición no exista
    if (mbr.mbr_partition_1.part_name == particion.part_name){
        cout << "¡Error! Ya hay una partición con ese nombre. No seas poco creativo y ponle otro." << endl;
        return;
    } else if (mbr.mbr_partition_2.part_name == particion.part_name){
        cout << "¡Error! Ya hay una partición con ese nombre. No seas poco creativo y ponle otro." << endl;
        return;
    } else if (mbr.mbr_partition_3.part_name == particion.part_name){
        cout << "¡Error! Ya hay una partición con ese nombre. No seas poco creativo y ponle otro." << endl;
        return;
    } else if (mbr.mbr_partition_4.part_name == particion.part_name){
        cout << "¡Error! Ya hay una partición con ese nombre. No seas poco creativo y ponle otro." << endl;
        return;
    }

    // Si es una partición extendida
    if (particion.part_type == 'e' || particion.part_type == 'E'){
        // Verificamos que no exista una partición extendida
        if (mbr.mbr_partition_1.part_type == 'e' || mbr.mbr_partition_1.part_type == 'E'){
            cout << "¡Error! Ya existe una partición extendida en este disco. Imposible crear otra." << endl;
            return;
        } else if (mbr.mbr_partition_2.part_type == 'e' || mbr.mbr_partition_2.part_type == 'E'){
            cout << "¡Error! Ya existe una partición extendida en este disco. Imposible crear otra." << endl;
            return;
        } else if (mbr.mbr_partition_3.part_type == 'e' || mbr.mbr_partition_3.part_type == 'E'){
            cout << "¡Error! Ya existe una partición extendida en este disco. Imposible crear otra." << endl;
            return;
        } else if (mbr.mbr_partition_4.part_type == 'e' || mbr.mbr_partition_4.part_type == 'E'){
            cout << "¡Error! Ya existe una partición extendida en este disco. Imposible crear otra." << endl;
            return;
        }
    } 

    // Si es una partición lógica
    if (particion.part_type == 'l' || particion.part_type == 'L'){
        // Verificamos que exista una partición extendida
        if (mbr.mbr_partition_1.part_type != 'e' && mbr.mbr_partition_1.part_type != 'E' &&
            mbr.mbr_partition_2.part_type != 'e' && mbr.mbr_partition_2.part_type != 'E' &&
            mbr.mbr_partition_3.part_type != 'e' && mbr.mbr_partition_3.part_type != 'E' &&
            mbr.mbr_partition_4.part_type != 'e' && mbr.mbr_partition_4.part_type != 'E'){
            cout << "¡Error! La partición Lógica se siente sola sin su amiga Extendida" << endl;
            return;
        }
    }

    // Si es una partición primaria
    if (particion.part_type == 'p' || particion.part_type == 'P' || (particion.part_type != 'e' && particion.part_type != 'E' && particion.part_type != 'l' && particion.part_type != 'L')){
        cout << "Primaria" << endl;
        mejor_ajuste(particion_comando, particion);
        cout << "Prueba primaria" << endl;
    }

}

// Función para confirmar la eliminación de una partición
bool confirmar_eliminación(){
    char respuesta;
    cout << "Eliminar esta partición no tendrá marcha atrás. ¿Estás preparado? (s/n): ";
    cin >> respuesta;
    if (respuesta == 's' || respuesta == 'S'){
        return true;
    } else {
        return false;
    }
}

// Función para eliminar una partición
void delete_partition(fdisk *partition){

    // Abrimos el mbr
    FILE *archivo;
    archivo = fopen(partition->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "Prueba 3" << endl;
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }

    // Leemos el mbr
    MBR mbr;
    fseek(archivo, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Verificamos que la partición exista
    if (mbr.mbr_partition_1.part_name == partition->name && confirmar_eliminación()){
        mbr.mbr_partition_1 = {};
    } else if (mbr.mbr_partition_2.part_name == partition->name && confirmar_eliminación()){
        mbr.mbr_partition_2 = {};
    } else if (mbr.mbr_partition_3.part_name == partition->name && confirmar_eliminación()){
        mbr.mbr_partition_3 = {};
    } else if (mbr.mbr_partition_4.part_name == partition->name && confirmar_eliminación()){
        mbr.mbr_partition_4 = {};
    } else {
        cout << "¡Error! Esta partición es como Pie Grande, muchos dicen que existe pero nadie la ha visto." << endl;
        return;
    }

    // Escribimos el mbr
    fseek(archivo, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, archivo);
    fclose(archivo);
}

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
        cout << "Prueba 4" << endl;
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }

    // Abrimos el mbr
    MBR mbr;
    FILE *archivo;
    archivo = fopen(partition_comando->path.c_str(), "rb+");
    if (archivo == NULL)
    {
        cout << "Prueba 5" << endl;
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }
    fread(&mbr, sizeof(MBR), 1, archivo);

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
        cout << "¡Partición creada con éxito!" << endl;

        // Leemos el mbr
        fseek(archivo, 0, SEEK_SET);
        fread(&mbr, sizeof(MBR), 1, archivo);
        cout << "mbr_partition_1.part_name: " << mbr.mbr_partition_1.part_name << endl;
        cout << "mbr_partition_2.part_name: " << mbr.mbr_partition_2.part_name << endl;
        cout << "mbr_partition_3.part_name: " << mbr.mbr_partition_3.part_name << endl;
        cout << "mbr_partition_4.part_name: " << mbr.mbr_partition_4.part_name << endl;
        fclose(archivo);
    } else {
        cout << "¡Error! No tengo ni idea de lo que intentas hacer con esa partición. ¿Podrías darme una pista?" << endl;
        return;
    }

    

    

}