/**
 * It creates a partition
 */
#include "fdisk.h"
#include "../mkdisk/mkdisk.h"
#include "ebr.h"
#include <iostream>
#include <string.h>

using namespace std;

fdisk::fdisk() {}

EBR::EBR() {}

/**
 * Abre el archivo, lee el MBR, encuentra la partición, aumenta o disminuye el tamaño de la partición, guarda los cambios y cierra el archivo.
 * 
 * @param particion_comando La partición que se va a modificar.
 * 
 */
void agregar_espacio(fdisk* particion_comando) {
    // Abrimos el archivo en modo lectura/escritura binario
    FILE* archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (!archivo) {
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }

    // Leemos el MBR
    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Buscamos la partición
    Partition* particion = NULL;
    particion = mbr.findPartition(particion_comando->name);
    

    // Si no encontramos la partición, mostramos un mensaje de error
    if (!particion) {
        cout << "¡Error! No se ha encontrado la partición. Quizás se haya ido a pescar con su familia" << endl;
        fclose(archivo);
        return;
    }

    // Aumentamos o disminuimos el tamaño de la partición
    //Si se aumenta 
    if (particion_comando->add > 0) {
        // Verificamos el espacio después de la partición
        Partition* particiones[] = {&mbr.mbr_partition_1, &mbr.mbr_partition_2, &mbr.mbr_partition_3, &mbr.mbr_partition_4};
        int espacio_despues = 0;
        for (int i = 0; i < 3; i++) {
            if (particion == particiones[i]) {
                if (i == 3) {
                    espacio_despues = mbr.mbr_tamano - (particion->part_start + particion->part_size);
                } else {
                    espacio_despues = (particiones[i+1]->part_start + particiones[i+1]->part_size) - (particion->part_start + particion->part_size);
                }
                break;
            }
        }
        // Si no hay espacio después de la partición, mostramos un mensaje de error
        if (espacio_despues < particion_comando->add) {
            cout << "¡Error! Parece que la partición se ha puesto a dieta y no quiere aumentar de tamaño." << endl;
            fclose(archivo);
            return;
        }
        // Aumentamos el tamaño de la partición
        particion->part_size += particion_comando->add;
        // Quitamos el espacio después de la partición
        for (int i = 0; i < 3; i++) {
            if (particion == particiones[i]) {
                if (i != 3) {
                    particiones[i+1]->part_start = particiones[i]->part_start + particiones[i]->part_size;
                    particiones[i+1]->part_size -= particion_comando->add;
                }
                break;
            }
        }
    } else if (particion_comando->add < 0) {
        // Si se disminuye
        // Verificamos el tamaño de la partición
        if (particion->part_size + particion_comando->add < 0) {
            cout << "¡Error! ¿Un tamaño negativo? Eso no está bien, amigo." << endl;
            fclose(archivo);
            return;
        }

        // Disminuimos el tamaño de la partición
        particion->part_size += particion_comando->add;
        
    }

    // Guardamos los cambios
    fseek(archivo, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, archivo);
    fclose(archivo);
    cout << "¡Partición modificada con éxito!" << endl;
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
    for (int i = 0; i < 3; i++){
        if (espacios_libres[i].partition->part_name[0] != '\0'){
            espacios_libres[i].size = -1;
        }else if (espacios_libres[i].partition->part_size != -1 && espacios_libres[i].partition->part_start != -1){
            if (i != 3){
                espacios_libres[i].size = espacios_libres[i + 1].partition->part_start - espacios_libres[i].partition->part_start;
            } else {
                espacios_libres[i].size = mbr.mbr_tamano - espacios_libres[i].partition->part_start;
            }
            if (espacios_libres[i].size  == 0){
                espacios_libres[i].size = -1;
            }
        }
    }

    // Comparamos los espacios libres el más cercano al tamaño de la partición nueva
    int mejor_ajuste = -1;
    for (int i = 0; i < 3; i++){
        if (espacios_libres[i].size >= tamano && espacios_libres[i].size != -1){
            if (mejor_ajuste == -1){
                mejor_ajuste = espacios_libres[i].size;
            } else if (espacios_libres[i].size < mejor_ajuste){
                mejor_ajuste = espacios_libres[i].size;
            }
        }
    }

    // Si encontramos el mejor ajuste, asignamos la partición
    // Para las particoines parimarias 
    if (particion_comando->type == 'p'){
        if (mejor_ajuste != -1){
            for (int i = 0; i < 3; i++){
                if (espacios_libres[i].size == mejor_ajuste){
                    particion.part_start = espacios_libres[i].partition->part_start;
                    particion.part_size = tamano;
                    // Guardamos la partición
                    *(espacios_libres[i].partition) = particion;
                    // Guardamos los cambios
                    fseek(archivo, 0, SEEK_SET);
                    fwrite(&mbr, sizeof(MBR), 1, archivo);
                    fclose(archivo);
                    cout << "¡Partición creada con éxito!" << endl;
                    return;
                }
            }
        } else {
            cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
            fclose(archivo);
            return;
        }
    } else if (particion_comando->type == 'e'){
        // Para las particiones extendidas
        if (mejor_ajuste != -1){
            for (int i = 0; i < 3; i++){
                if (espacios_libres[i].size == mejor_ajuste){
                    particion.part_start = espacios_libres[i].partition->part_start;
                    particion.part_size = tamano;
                    // Guardamos la partición
                    *(espacios_libres[i].partition) = particion;
                    // Guardamos los cambios
                    fseek(archivo, 0, SEEK_SET);
                    fwrite(&mbr, sizeof(MBR), 1, archivo);
                    fclose(archivo);
                    cout << "¡Partición creada con éxito!" << endl;
                    return;
                }
            }
        } else {
            cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
            fclose(archivo);
            return;
        }
    } else if (particion_comando->type == 'l'){
        // Para las particiones lógicas
        // Buscamos la partición extendida
        Partition *extendida = NULL;
        for (int i = 0; i < 3; i++){
            if (espacios_libres[i].partition->part_type == 'e'){
                extendida = espacios_libres[i].partition;
                break;
            }
        }
        if (extendida == NULL){
            cout << "¡Error! No se ha encontrado una partición extendida." << endl;
            fclose(archivo);
            return;
        }
        // Abrimos la partición extendida
        FILE *archivo_extendida;
        archivo_extendida = fopen(particion_comando->path.c_str(), "rb+");
        if (archivo_extendida == NULL)
        {
            cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
            return;
        }
        // Leemos la partición extendida
        EBR ebr;
        fseek(archivo_extendida, extendida->part_start, SEEK_SET);
        fread(&ebr, sizeof(EBR), 1, archivo_extendida);
        // Buscamos el último ebr
        while (ebr.part_next != -1){
            fseek(archivo_extendida, ebr.part_next, SEEK_SET);
            fread(&ebr, sizeof(EBR), 1, archivo_extendida);
        }
        // Asignamos la partición
        particion.part_start = ebr.part_start + ebr.part_size;
        ebr.part_next = particion.part_start;
        fseek(archivo_extendida, ebr.part_start, SEEK_SET);
        fwrite(&ebr, sizeof(EBR), 1, archivo_extendida);
        fclose(archivo_extendida);

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
    for (int i = 0; i < 3; i++){
        if (espacios_libres[i].partition->part_name[0] != '\0'){
            espacios_libres[i].size = -1;
        }else if (espacios_libres[i].partition->part_size != -1 && espacios_libres[i].partition->part_start != -1){
            if (i != 3){
                espacios_libres[i].size = espacios_libres[i + 1].partition->part_start - espacios_libres[i].partition->part_start;
            } else {
                espacios_libres[i].size = mbr.mbr_tamano - espacios_libres[i].partition->part_start;
            }
            if (espacios_libres[i].size  == 0){
                espacios_libres[i].size = -1;
            }
        }
    }

    // Buscamos el espacio libre más grande
    int peor_ajuste = -1;
    for (int i = 0; i < 3; i++){
        if (espacios_libres[i].size >= tamano && espacios_libres[i].size != -1){
            if (peor_ajuste == -1){
                peor_ajuste = espacios_libres[i].size;
            } else if (espacios_libres[i].size > peor_ajuste){
                peor_ajuste = espacios_libres[i].size;
            }
        }
    }

    // Si encontramos el mejor ajuste, asignamos la partición
    // Para las particiones primarias
    if (particion.part_type == 'p'){
        for (int i = 0; i < 3; i++){
            if (espacios_libres[i].size == peor_ajuste && espacios_libres[i].partition->part_name[0] == '\0'){
                particion.part_start = espacios_libres[i].partition->part_start;
                particion.part_size = tamano;
                // Asignamos la partición
                *(espacios_libres[i].partition) = particion;
                // Escribimos el mbr
                fseek(archivo, 0, SEEK_SET);
                fwrite(&mbr, sizeof(MBR), 1, archivo);
                fclose(archivo);   
                cout << "¡Partición creada con éxito!" << endl;
                return;
            }
        }
    } else if (particion.part_type == 'e'){
        // Para las particiones extendidas
        for (int i = 0; i < 3; i++){
            if (espacios_libres[i].size == peor_ajuste && espacios_libres[i].partition->part_name[0] == '\0'){
                particion.part_start = espacios_libres[i].partition->part_start;
                particion.part_size = tamano;
                // Asignamos la partición
                *(espacios_libres[i].partition) = particion;
                // Escribimos el mbr
                fseek(archivo, 0, SEEK_SET);
                fwrite(&mbr, sizeof(MBR), 1, archivo);
                fclose(archivo);   
                cout << "¡Partición creada con éxito!" << endl;
                return;
            }
        }
    } else if (particion.part_type == 'l'){
        // Para las particiones lógicas
        // Buscamos la partición extendida
        Partition* particion_extendida;
        if (mbr.mbr_partition_1.part_type == 'e'){
            particion_extendida = &mbr.mbr_partition_1;
        } else if (mbr.mbr_partition_2.part_type == 'e'){
            particion_extendida = &mbr.mbr_partition_2;
        } else if (mbr.mbr_partition_3.part_type == 'e'){
            particion_extendida = &mbr.mbr_partition_3;
        } else if (mbr.mbr_partition_4.part_type == 'e'){
            particion_extendida = &mbr.mbr_partition_4;
        } else {
            cout << "¡Error! No se ha encontrado una partición extendida." << endl;
            fclose(archivo);
            return;
        }
        // Abrimos la partición extendida
        FILE *archivo_extendida;
        archivo_extendida = fopen(particion_comando->path.c_str(), "rb+");
        if (archivo_extendida == NULL)
        {
            cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
            fclose(archivo);
            return;
        }
        // Leemos el ebr
        EBR ebr;
        fseek(archivo_extendida, particion_extendida->part_start, SEEK_SET);
        fread(&ebr, sizeof(EBR), 1, archivo_extendida);
        // Buscamos el primer espacio libre
        while (ebr.part_next != -1){
            fseek(archivo_extendida, ebr.part_next, SEEK_SET);
            fread(&ebr, sizeof(EBR), 1, archivo_extendida);
        }
        // Guardamos la partición
        ebr.part_status = '1';
        strcpy(ebr.part_name, particion.part_name);
        ebr.part_fit = particion.part_fit;
        ebr.part_size = particion.part_size;
        ebr.part_start = ebr.part_next;
        ebr.part_next = -1;
        // Guardamos los cambios
        fseek(archivo_extendida, ebr.part_start, SEEK_SET);
        fwrite(&ebr, sizeof(EBR), 1, archivo_extendida);
        fclose(archivo_extendida);
        fclose(archivo);
        cout << "¡Partición creada con éxito!" << endl;
        return;
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
    for (int i = 0; i < 3; i++){
        if (espacios_libres[i].partition->part_name[0] != '\0'){
            espacios_libres[i].size = -1;
        }else if (espacios_libres[i].partition->part_size != -1 && espacios_libres[i].partition->part_start != -1){
            if (i != 3){
                espacios_libres[i].size = espacios_libres[i+1].partition->part_start - espacios_libres[i].partition->part_start;
            } else {
                espacios_libres[i].size = mbr.mbr_tamano - espacios_libres[i].partition->part_start;
            }
            if (espacios_libres[i].size  == 0){
                espacios_libres[i].size = -1;
            }
        }
    }

    // Buscamos el primer espacio libre
    // Para las parciones primarias
    if(particion.part_type == 'p'){
        for (int i = 0; i < 3; i++){
            if (espacios_libres[i].size >= tamano && espacios_libres[i].size != -1){
                particion.part_start = espacios_libres[i].partition->part_start;
                particion.part_size = espacios_libres[i].partition->part_size;
                // Guardamos la partición
                *(espacios_libres[i].partition) = particion;
                // Guardamos los cambios
                fseek(archivo, 0, SEEK_SET);
                fwrite(&mbr, sizeof(MBR), 1, archivo);
                fclose(archivo);
                cout << "¡Partición creada con éxito!" << endl;
                return;
            }
        }
    } else if (particion.part_type == 'e'){
        // Para las particiones extendidas
        for (int i = 0; i < 3; i++){
            if (espacios_libres[i].size >= tamano && espacios_libres[i].size != -1){
                particion.part_start = espacios_libres[i].partition->part_start;
                particion.part_size = espacios_libres[i].partition->part_size;
                // Guardamos la partición
                *(espacios_libres[i].partition) = particion;
                // Guardamos los cambios
                fseek(archivo, 0, SEEK_SET);
                fwrite(&mbr, sizeof(MBR), 1, archivo);
                fclose(archivo);
                cout << "¡Partición creada con éxito!" << endl;
                return;
            }
        }
    } else if (particion.part_type == 'l'){
        // Para las particiones lógicas
        // Buscamos las particiones extendidas
        for (int i = 0; i < 3; i++){
            if (espacios_libres[i].partition->part_type == 'e'){
                // Leemos la partición extendida
                fseek(archivo, espacios_libres[i].partition->part_start, SEEK_SET);
                EBR ebr;
                fread(&ebr, sizeof(EBR), 1, archivo);
                // Buscamos el último ebr
                while (ebr.part_next != -1){
                    fseek(archivo, ebr.part_next, SEEK_SET);
                    fread(&ebr, sizeof(EBR), 1, archivo);
                }
                // Si el último ebr está vacío, lo llenamos
                if (ebr.part_name[0] == '\0'){
                    ebr.part_status = '1';
                    ebr.part_fit = particion.part_fit;
                    ebr.part_start = ebr.part_next;
                    ebr.part_size = particion.part_size;
                    strcpy(ebr.part_name, particion.part_name);
                    ebr.part_next = -1;
                    fseek(archivo, ebr.part_start, SEEK_SET);
                    fwrite(&ebr, sizeof(EBR), 1, archivo);
                    fclose(archivo);
                    cout << "¡Partición creada con éxito!" << endl;
                    return;
                }
            }
        }
    }

    // Si no encontramos un espacio libre adecuado, mostramos un mensaje de error
    cout << "¡Error! El disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
    fclose(archivo);    
}
    


// Función para crear una partición
/**
 * Crea una partición
 * 
 * @param particion La partición que se va a crear.
 * @param particion_comando El comando que el usuario ingresó.
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
    for (int i = 0; i < 3; i++) {
        if (strcmp(mbr.getPartition(i).part_name, particion.part_name) == 0) {
            cout << "¡Error! Ya hay una partición con ese nombre. No seas poco creativo y ponle otro." << endl;
            return;
        }
    }

    // Si es una partición extendida
    if (particion.part_type == 'e' || particion.part_type == 'E') {
        // Verificamos que no exista una partición extendida
        for (int i = 0; i < 3; i++) {
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
        for (int i = 0; i < 3; i++) {
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
        // Verificamos el fit
        if (particion.part_fit == 'f' || particion.part_fit == 'F'){
            primer_ajuste(particion_comando,particion);
        } else if (particion.part_fit == 'b' || particion.part_fit == 'B'){
            mejor_ajuste(particion_comando,particion);
        } else if (particion.part_fit == 'w' || particion.part_fit == 'W'){
            peor_ajuste(particion_comando,particion);
        } else {
            cout << "¡Error! El fit no es válido. Debe ser ff, bf o wf." << endl;
            return;
        }

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
        int tmpInicio = particion->part_start;
        int tmpSize = particion->part_size;
        *particion = {};
        particion->part_start = tmpInicio;
        particion->part_size = tmpSize;
        particion->part_name[0] = '\0';
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

    // Size
    if (partition_comando->unit == 'k' || partition_comando->unit == 'K'){
        // KiloBytes
        partition_comando->size = partition_comando->size * 1024;
    } else if (partition_comando->unit == 'm' || partition_comando->size == 'M'){
        // MegaBytes
        partition_comando->size = partition_comando->add * 1024 * 1024;
    } else if (partition_comando->unit == 'b' || partition_comando->size == 'B'){
        // Bytes
        partition_comando->size = partition_comando->size;
    } else {
        cout << "¡Error! La unidad no es válida. Debe ser k, m o b." << endl;
        return;
    }


    // Verificamos qué se va a hacer con la partición
    if (partition_comando->delete_ == "full" || partition_comando->delete_ == "FULL"){
        delete_partition(partition_comando);
    } else if (partition_comando->add != 0){
        if (partition_comando->unit == 'k' || partition_comando->unit == 'K'){
            // KiloBytes
            partition_comando->add = partition_comando->add * 1024;
        } else if (partition_comando->unit == 'm' || partition_comando->size == 'M'){
            // MegaBytes
            partition_comando->add = partition_comando->add * 1024 * 1024;
        } else if (partition_comando->unit == 'b' || partition_comando->size == 'B'){
            // Bytes
            partition_comando->add = partition_comando->add;
        } else {
            cout << "¡Error! La unidad no es válida. Debe ser k, m o b." << endl;
            return;
        }
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
    cout << "Start: " << mbr.mbr_partition_1.part_start << endl;
    cout << "=====Partición 2=====" << endl;
    cout << "Nombre: " << mbr.mbr_partition_2.part_name << endl;
    cout << "Tamaño: " << mbr.mbr_partition_2.part_size << endl;
    cout << "Tipo: " << mbr.mbr_partition_2.part_type << endl;
    cout << "Ajuste: " << mbr.mbr_partition_2.part_fit << endl;
    cout << "Status: " << mbr.mbr_partition_2.part_status << endl;
    cout << "Start: " << mbr.mbr_partition_2.part_start << endl;
    cout << "=====Partición 3=====" << endl;
    cout << "Nombre: " << mbr.mbr_partition_3.part_name << endl;
    cout << "Tamaño: " << mbr.mbr_partition_3.part_size << endl;
    cout << "Tipo: " << mbr.mbr_partition_3.part_type << endl;
    cout << "Ajuste: " << mbr.mbr_partition_3.part_fit << endl;
    cout << "Status: " << mbr.mbr_partition_3.part_status << endl;
    cout << "Start: " << mbr.mbr_partition_3.part_start << endl;
    cout << "=====Partición 4=====" << endl;
    cout << "Nombre: " << mbr.mbr_partition_4.part_name << endl;
    cout << "Tamaño: " << mbr.mbr_partition_4.part_size << endl;
    cout << "Tipo: " << mbr.mbr_partition_4.part_type << endl;
    cout << "Ajuste: " << mbr.mbr_partition_4.part_fit << endl;
    cout << "Status: " << mbr.mbr_partition_4.part_status << endl;
    cout << "Start: " << mbr.mbr_partition_4.part_start << endl;


    fclose(archivo);

    

}