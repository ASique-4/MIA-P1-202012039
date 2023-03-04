/**
 * It creates a partition
 */
#include "fdisk.h"
#include "../mkdisk/mkdisk.h"
#include "ebr.h"
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

fdisk::fdisk() {}

EBR::EBR() {}

struct EspacioLibre
{
    int start;
    int size;
    Partition *partition;
};

/**
 * Devuelve un puntero a la primera partición en el MBR que está vacía
 *
 * @param mbr El MBR del disco.
 *
 * @return Un puntero a una partición.
 */
Partition* buscarParticionLibre(MBR& mbr) {
    Partition* particionLibre = nullptr;

    Partition* particiones[4] = {&mbr.mbr_partition_1, &mbr.mbr_partition_2, &mbr.mbr_partition_3, &mbr.mbr_partition_4};

    for (int i = 0; i < 4; i++) {
        if (particiones[i]->part_name[0] == '\0') {
            particionLibre = particiones[i];
            break;
        }
    }
    return particionLibre;
}


/**
 * Escribe el MBR al principio del archivo.
 *
 * @param mbr La estructura MBR que se va a guardar.
 * @param archivo El archivo a escribir.
 */
void guardarMBR(MBR mbr, FILE *archivo)
{
    fseek(archivo, 0, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, archivo);
    fclose(archivo);
}

/**
 * Toma un MBR y devuelve un vector de estructuras de EspacioLibre, las cuales contienen el tamaño y
 * el puntero de partición de cada espacio libre en el disco
 *
 * @param mbr El MBR del disco.
 *
 * @return Un vector de estructuras de EspacioLibre.
 */
vector<EspacioLibre> calcularEspaciosLibres(MBR mbr)
{
    // Creamos un vector para almacenar los espacios libres
    vector<EspacioLibre> espacios_libres;

    // Creamos un vector de punteros a las particiones
    vector<Partition *> particiones = {&mbr.mbr_partition_1, &mbr.mbr_partition_2, &mbr.mbr_partition_3, &mbr.mbr_partition_4};

    // Ordenamos las particiones por posición de inicio
    mbr.OrdenarParticiones();

    // Iteramos sobre las particiones para encontrar los espacios libres
    int last_end = 0;
    for (int i = 0; i < 4; i++)
    {
        // Si la partición tiene un nombre, no es un espacio libre
        if (particiones[i]->part_name[0] != '\0')
        {
            // Si la partición comienza más allá del final de la última partición, hay un espacio libre entre ellas
            if (particiones[i]->part_start > last_end)
            {
                espacios_libres.push_back({particiones[i]->part_start - last_end, last_end, nullptr});
            }
            // Actualizamos el final de la última partición
            last_end = particiones[i]->part_start + particiones[i]->part_size;
        }
        // Si la partición no tiene nombre, es un espacio libre
        else
        {
            espacios_libres.push_back({particiones[i]->part_size, particiones[i]->part_start, nullptr});
            // Actualizamos el final de la última partición
            last_end = particiones[i]->part_start + particiones[i]->part_size;
        }
    }

    // Si hay un espacio libre al final del disco, lo agregamos
    if (last_end < mbr.mbr_tamano)
    {
        espacios_libres.push_back({mbr.mbr_tamano - last_end, last_end, nullptr});
    }

    // Retornamos el vector de espacios libres
    return espacios_libres;
}

/**
 * Encuentra el índice del espacio libre más pequeño que puede caber en el archivo
 *
 * @param espacios_libres un vector de objetos de EspacioLibre
 * @param tam_archivo El tamaño del archivo que se va a asignar.
 *
 * @return El índice de mejor ajuste.
 */
int buscarIndexMejorAjuste(vector<EspacioLibre> espacios_libres, int tam_archivo)
{
    int mejor_ajuste = -1;

    for (int i = 0; i < espacios_libres.size(); i++)
    {
        if (espacios_libres[i].size >= tam_archivo)
        {
            if (mejor_ajuste == -1 || espacios_libres[i].size < espacios_libres[mejor_ajuste].size)
            {
                mejor_ajuste = i;
            }
        }
    }

    return mejor_ajuste;
}



/**
 * Encuentra el índice del espacio peor ajustado en el vector de espacios libres
 * 
 * @param espacios_libres Un vector de espacios libres.
 * @param tam_archivo El tamaño del archivo que se va a asignar.
 * 
 * @return El índice del peor ajuste.
 */
int buscarIndexPeorAjuste(vector<EspacioLibre> espacios_libres, int tam_archivo)
{
    int peor_ajuste = -1;

    for (int i = 0; i < espacios_libres.size(); i++)
    {
        if (espacios_libres[i].size >= tam_archivo)
        {
            if (peor_ajuste == -1 || espacios_libres[i].size > espacios_libres[peor_ajuste].size)
            {
                peor_ajuste = i;
            }
        } else if (peor_ajuste == -1) {
            peor_ajuste = i;
        }
    }
    

    return peor_ajuste;
}

/**
 * Devuelve el índice del primer espacio libre que es lo suficientemente grande para contener el
 * archivo
 * 
 * @param espacios_libres un vector de objetos de EspacioLibre.
 * @param tam_archivo El tamaño del archivo que se va a asignar.
 * 
 * @return El índice del primer espacio libre que es mayor o igual que el tamaño del archivo.
 */
int buscarIndexPrimerAjuste(vector<EspacioLibre> espacios_libres, int tam_archivo)
{
    for (int i = 0; i < espacios_libres.size(); i++)
    {
        if (espacios_libres[i].size >= tam_archivo)
        {
            return i;
        }
    }

    return -1;
}


/**
 * Devuelve el inicio de la partición dado el índice de la partición.
 *
 * @param i El número de partición.
 * @param mbr El MBR del disco
 *
 * @return El comienzo de la partición.
 */
int getStart(char name[16], MBR mbr)
{
    Partition *particiones[4] = {&mbr.mbr_partition_1, &mbr.mbr_partition_2, &mbr.mbr_partition_3, &mbr.mbr_partition_4};
    
    for (int i = 0; i < 4; i++)
    {
        if (strcmp(particiones[i]->part_name, name) == 0)
        {
            if (i == 0)
            {
                return sizeof(MBR);
            }
            else
            {
                return getStart(particiones[i - 1]->part_name, mbr) + particiones[i - 1]->part_size;
            }
        }
    }
    return -1;
}

/**
 * Abre el archivo, lee el MBR, encuentra la partición, aumenta o disminuye el tamaño de la partición, guarda los cambios y cierra el archivo.
 *
 * @param particion_comando La partición que se va a modificar.
 *
 */
void agregar_espacio(fdisk *particion_comando)
{
    // Abrimos el archivo en modo lectura/escritura binario
    FILE *archivo = fopen(particion_comando->path.c_str(), "rb+");
    if (!archivo)
    {
        cout << "¡Error! No se ha encontrado el archivo. Quizás se haya ido a pescar con su familia" << endl;
        return;
    }

    // Leemos el MBR
    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, archivo);

    // Buscamos la partición
    Partition *particion = NULL;
    particion = mbr.findPartition(particion_comando->name);

    // Si no encontramos la partición, mostramos un mensaje de error
    if (!particion)
    {
        cout << "¡Error! No se ha encontrado la partición. Quizás se haya ido a pescar con su familia" << endl;
        fclose(archivo);
        return;
    }

    // Aumentamos o disminuimos el tamaño de la partición
    // Si se aumenta
    if (particion_comando->add > 0)
    {
        // Verificamos el espacio después de la partición
        Partition *particiones[] = {&mbr.mbr_partition_1, &mbr.mbr_partition_2, &mbr.mbr_partition_3, &mbr.mbr_partition_4};
        int espacio_despues = 0;
        for (int i = 0; i < 4; i++)
        {
            if (particion == particiones[i])
            {
                if (i == 3)
                {
                    espacio_despues = mbr.mbr_tamano - (particion->part_start + particion->part_size);
                }
                else
                {
                    espacio_despues = (particiones[i + 1]->part_start + particiones[i + 1]->part_size) - (particion->part_start + particion->part_size);
                }
                break;
            }
        }
        // Si no hay espacio después de la partición, mostramos un mensaje de error
        if (espacio_despues < particion_comando->add)
        {
            cout << "¡Error! Parece que la partición se ha puesto a dieta y no quiere aumentar de tamaño." << endl;
            fclose(archivo);
            return;
        }
        // Aumentamos el tamaño de la partición
        particion->part_size += particion_comando->add;
        // Quitamos el espacio después de la partición
        for (int i = 0; i < 4; i++)
        {
            if (particion == particiones[i])
            {
                if (i != 3)
                {
                    particiones[i + 1]->part_start = particiones[i]->part_start + particiones[i]->part_size;
                    particiones[i + 1]->part_size -= particion_comando->add;
                }
                break;
            }
        }
    }
    else if (particion_comando->add < 0)
    {
        // Si se disminuye
        // Verificamos el tamaño de la partición
        if (particion->part_size + particion_comando->add < 0)
        {
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
void mejor_ajuste(fdisk *particion_comando, Partition particion)
{

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

    // Creamos una lista de espacios libres
    vector<EspacioLibre> espacios_libres = calcularEspaciosLibres(mbr);

    // Buscamos el mejor ajuste
    int best_fit_index = buscarIndexMejorAjuste(espacios_libres, tamano);

    // Si encontramos el mejor ajuste, asignamos la partición
    // Para las particiones parimarias
    if (best_fit_index != -1)
    {
        if (particion.part_type == 'P')
        {
            if (espacios_libres[best_fit_index].partition != nullptr)
            {
                espacios_libres[best_fit_index].partition->part_size = tamano;
                espacios_libres[best_fit_index].partition->part_status = '0';
                espacios_libres[best_fit_index].partition->part_type = 'P';
                espacios_libres[best_fit_index].partition->part_fit = particion.part_fit;
                strcpy(espacios_libres[best_fit_index].partition->part_name, particion.part_name);
            }
            else
            {
                // Buscamos una partición libre
                Partition *particionLibre = buscarParticionLibre(mbr);
                // Si no encontramos una partición libre, mostramos un mensaje de error
                if (particionLibre == nullptr)
                {
                    cout << "¡Error! Parece que el disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
                    fclose(archivo);
                    return;
                }

                // Asignamos la partición
                particionLibre->part_size = tamano;
                particionLibre->part_status = '0';
                particionLibre->part_type = 'P';
                particionLibre->part_fit = particion.part_fit;
                strcpy(particionLibre->part_name, particion.part_name);
                particionLibre->part_start = getStart(particion.part_name, mbr);
            }
        } else if (particion.part_type == 'E')
        {
            // Para las particiones extendidas
            // Buscamos una partición libre
            Partition *particionLibre = buscarParticionLibre(mbr);
            // Si no encontramos una partición libre, mostramos un mensaje de error
            if (particionLibre == nullptr)
            {
                cout << "¡Error! Parece que el disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
                fclose(archivo);
                return;
            }

            // Asignamos la partición
            particionLibre->part_size = tamano;
            particionLibre->part_status = '0';
            particionLibre->part_type = 'E';
            particionLibre->part_fit = particion.part_fit;
            strcpy(particionLibre->part_name, particion.part_name);
            particionLibre->part_start = getStart(particion.part_name, mbr);
        }  
    }

    // Ordenamos las particiones
    mbr.OrdenarParticiones();

    // Guardamos los cambios
    guardarMBR(mbr, archivo);
}

/**
 * Encuentra el mayor espacio libre en el disco y le asigna la partición
 *
 * @param particion_comando La estructura fdisk que contiene la ruta al disco y el nombre de la
 * partición.
 * @param particion La partición a crear.
 *
 */
void peor_ajuste(fdisk *particion_comando, Partition particion)
{

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

    // Copiamos el array de calcularEspaciosLibres a espacio_libre
    vector<EspacioLibre> espacios_libres = calcularEspaciosLibres(mbr);
    for (int i = 0; i < espacios_libres.size(); i++)
    {
        cout << "Espacio libre: " << espacios_libres[i].size << endl;
        cout << "Partición: " << i << endl;
        cout << "Start: " << espacios_libres[i].start << endl;
    }

    // Buscamos el mejor ajuste
    int best_fit_index = buscarIndexPeorAjuste(espacios_libres, tamano);
    cout << "El mejor ajuste es: " << best_fit_index << endl;
    

    // Si encontramos el mejor ajuste, asignamos la partición
    // Para las particiones parimarias
    if (best_fit_index != -1)
    {
        if (particion.part_type == 'P' || particion.part_type == 'p')
        {
            if (espacios_libres[best_fit_index].partition != nullptr)
            {
                espacios_libres[best_fit_index].partition->part_size = tamano;
                espacios_libres[best_fit_index].partition->part_status = '0';
                espacios_libres[best_fit_index].partition->part_type = 'P';
                espacios_libres[best_fit_index].partition->part_fit = particion.part_fit;
                strcpy(espacios_libres[best_fit_index].partition->part_name, particion.part_name);
            }
            else
            {
                // Buscamos una partición libre
                Partition *particionLibre = buscarParticionLibre(mbr);
                // Si no encontramos una partición libre, mostramos un mensaje de error
                if (particionLibre == nullptr)
                {
                    cout << "¡Error! Parece que el disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
                    fclose(archivo);
                    return;
                }

                // Asignamos la partición
                particionLibre->part_size = tamano;
                particionLibre->part_status = '0';
                particionLibre->part_type = 'P';
                particionLibre->part_fit = particion.part_fit;
                strcpy(particionLibre->part_name, particion.part_name);
                particionLibre->part_start = getStart(particion.part_name, mbr);

            }
        } else if (particion.part_type == 'E' || particion.part_type == 'e')
        {
            // Para las particiones extendidas
            // Buscamos una partición libre
            Partition *particionLibre = buscarParticionLibre(mbr);
            // Si no encontramos una partición libre, mostramos un mensaje de error
            if (particionLibre == nullptr)
            {
                cout << "¡Error! Parece que el disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
                fclose(archivo);
                return;
            }

            // Asignamos la partición
            particionLibre->part_size = tamano;
            particionLibre->part_status = '0';
            particionLibre->part_type = 'E';
            particionLibre->part_fit = particion.part_fit;
            strcpy(particionLibre->part_name, particion.part_name);
            particionLibre->part_start = getStart(particion.part_name, mbr);

        }
    }

    // Ordenamos las particiones
    mbr.OrdenarParticiones();

    // Guardamos los cambios
    guardarMBR(mbr, archivo);

}

/**
 * Encuentra el primer espacio libre en el disco y le asigna la partición
 *
 * @param particion_comando La estructura fdisk que contiene la ruta al disco y el nombre de la
 * partición.
 * @param particion La partición a crear.
 *
 */
void primer_ajuste(fdisk *particion_comando, Partition particion)
{

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
    vector<EspacioLibre> espacios_libres = calcularEspaciosLibres(mbr);

    // Buscamos el primer espacio libre
    int first_fit_index = buscarIndexPrimerAjuste(espacios_libres, tamano);

    // Si encontramos el primer espacio libre, asignamos la partición
    // Para las particiones primarias
    if (first_fit_index != -1)
    {
        if (particion.part_type == 'P')
        {
            if (espacios_libres[first_fit_index].partition != nullptr)
            {
                espacios_libres[first_fit_index].partition->part_size = tamano;
                espacios_libres[first_fit_index].partition->part_status = '0';
                espacios_libres[first_fit_index].partition->part_type = 'P';
                espacios_libres[first_fit_index].partition->part_fit = particion.part_fit;
                strcpy(espacios_libres[first_fit_index].partition->part_name, particion.part_name);
            }
            else
            {
                // Buscamos una partición libre
                Partition *particionLibre = buscarParticionLibre(mbr);
                // Si no encontramos una partición libre, mostramos un mensaje de error
                if (particionLibre == nullptr)
                {
                    cout << "¡Error! Parece que el disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
                    fclose(archivo);
                    return;
                }

                // Asignamos la partición
                particionLibre->part_size = tamano;
                particionLibre->part_status = '0';
                particionLibre->part_type = 'P';
                particionLibre->part_fit = particion.part_fit;
                strcpy(particionLibre->part_name, particion.part_name);
                particionLibre->part_start =getStart(particion.part_name, mbr);
            }
        } else if (particion.part_type == 'E')
        {
            // Para las particiones extendidas
            // Buscamos una partición libre
            Partition *particionLibre = buscarParticionLibre(mbr);
            // Si no encontramos una partición libre, mostramos un mensaje de error
            if (particionLibre == nullptr)
            {
                cout << "¡Error! Parece que el disco está tan lleno que las particiones están empezando a pelearse entre sí. No podemos crear otra." << endl;
                fclose(archivo);
                return;
            }

            // Asignamos la partición
            particionLibre->part_size = tamano;
            particionLibre->part_status = '0';
            particionLibre->part_type = 'E';
            particionLibre->part_fit = particion.part_fit;
            strcpy(particionLibre->part_name, particion.part_name);
            particionLibre->part_start = getStart(particion.part_name, mbr);

        }
    }

    // Ordenamos las particiones    
    mbr.OrdenarParticiones();

    // Guardamos los cambios
    guardarMBR(mbr, archivo);
}

// Función para crear una partición
/**
 * Crea una partición
 *
 * @param particion La partición que se va a crear.
 * @param particion_comando El comando que el usuario ingresó.
 *
 */
void crear_particion(Partition particion, fdisk *particion_comando)
{
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
    for (int i = 0; i < 4; i++)
    {
        if (strcmp(mbr.getPartition(i).part_name, particion.part_name) == 0)
        {
            cout << "¡Error! Ya hay una partición con ese nombre. No seas poco creativo y ponle otro." << endl;
            return;
        }
    }

    // Ordenamos las particiones por su start
    mbr.OrdenarParticiones();

    // Si es una partición extendida
    if (particion.part_type == 'e' || particion.part_type == 'E')
    {
        // Verificamos que no exista una partición extendida
        for (int i = 0; i < 4; i++)
        {
            if (mbr.getPartition(i).part_type == 'e' || mbr.getPartition(i).part_type == 'E')
            {
                cout << "¡Error! Ya existe una partición extendida en este disco. Imposible crear otra." << endl;
                return;
            }
        }
    }

    // Si es una partición lógica
    if (particion.part_type == 'l' || particion.part_type == 'L')
    {
        // Verificamos que exista una partición extendida
        bool has_extended_partition = false;
        for (int i = 0; i < 4; i++)
        {
            if (mbr.getPartition(i).part_type == 'e' || mbr.getPartition(i).part_type == 'E')
            {
                has_extended_partition = true;
                break;
            }
        }
        if (!has_extended_partition)
        {
            cout << "¡Error! La partición Lógica se siente sola sin su amiga Extendida" << endl;
            return;
        }
    }

    // Si es una partición primaria
    if (particion.part_type == 'p' || particion.part_type == 'P' || (particion.part_type != 'e' && particion.part_type != 'E' && particion.part_type != 'l' && particion.part_type != 'L'))
    {
        cout << "Primaria" << endl;
        cout << "Partición:" << mbr.getPartition(0).part_name << "." << endl;
        // Verificamos el fit
        if (particion.part_fit == 'f' || particion.part_fit == 'F')
        {
            primer_ajuste(particion_comando, particion);
        }
        else if (particion.part_fit == 'b' || particion.part_fit == 'B')
        {
            mejor_ajuste(particion_comando, particion);
        }
        else if (particion.part_fit == 'w' || particion.part_fit == 'W')
        {
            cout << "Peor ajuste" << endl;
            peor_ajuste(particion_comando, particion);
        }
        else
        {
            cout << "¡Error! El fit no es válido. Debe ser ff, bf o wf." << endl;
            return;
        }
    }
}

bool confirmar_eliminación()
{
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
void delete_partition(fdisk *partition)
{
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
    Partition *particion = mbr.findPartition(partition->name);
    if (particion != NULL && confirmar_eliminación())
    {
        int tmpInicio = particion->part_start;
        int tmpSize = particion->part_size;
        char tmpName[16];
        strcpy(particion->part_name, tmpName);
        *particion = {};
        particion->part_start = tmpInicio;
        particion->part_name[0] = '\0';
        particion->part_size = tmpSize;
        // Si hay espacio libre, lo unimos
        for (int i = 0; i < 4; i++)
        {
            Partition *currentPartition = mbr.findPartition(mbr.getPartition(i).part_name);
            if (currentPartition->part_start == particion->part_start + particion->part_size && currentPartition->part_name[0] == '\0')
            {
                particion->part_size += currentPartition->part_size;
                currentPartition = {};
                currentPartition->part_start = -1;
                currentPartition->part_name[0] = '\0';
                currentPartition->part_size = -1;
                break;
            }
            if (currentPartition->part_start > tmpInicio + tmpSize)
            {
                particion->part_size += currentPartition->part_start - (tmpInicio + tmpSize);
                break;
            }
        }
    }
    else
    {
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
    if (partition_comando->unit == 'k' || partition_comando->unit == 'K')
    {
        // KiloBytes
        partition_comando->size = partition_comando->size * 1024;
    }
    else if (partition_comando->unit == 'm' || partition_comando->size == 'M')
    {
        // MegaBytes
        partition_comando->size = partition_comando->add * 1024 * 1024;
    }
    else if (partition_comando->unit == 'b' || partition_comando->size == 'B')
    {
        // Bytes
        partition_comando->size = partition_comando->size;
    }
    else
    {
        cout << "¡Error! La unidad no es válida. Debe ser k, m o b." << endl;
        return;
    }

    // Verificamos qué se va a hacer con la partición
    if (partition_comando->delete_ == "full" || partition_comando->delete_ == "FULL")
    {
        delete_partition(partition_comando);
    }
    else if (partition_comando->add != 0)
    {
        if (partition_comando->unit == 'k' || partition_comando->unit == 'K')
        {
            // KiloBytes
            partition_comando->add = partition_comando->add * 1024;
        }
        else if (partition_comando->unit == 'm' || partition_comando->size == 'M')
        {
            // MegaBytes
            partition_comando->add = partition_comando->add * 1024 * 1024;
        }
        else if (partition_comando->unit == 'b' || partition_comando->size == 'B')
        {
            // Bytes
            partition_comando->add = partition_comando->add;
        }
        else
        {
            cout << "¡Error! La unidad no es válida. Debe ser k, m o b." << endl;
            return;
        }
        agregar_espacio(partition_comando);
    }
    else if (partition_comando->delete_ == "" && !isdigit(partition_comando->add))
    {
        Partition new_particion;
        strcpy(new_particion.part_name, partition_comando->name);
        new_particion.part_size = partition_comando->size;
        new_particion.part_fit = partition_comando->fit;
        new_particion.part_type = partition_comando->type;
        new_particion.part_status = '0';
        crear_particion(new_particion, partition_comando);
    }
    else
    {
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