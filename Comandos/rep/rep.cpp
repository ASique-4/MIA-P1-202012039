#include "rep.h"
#include "../fdisk/ebr.h"
#include "../mount/ListaDobleMount.h"
#include "../Estructuras/SuperBloque.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <filesystem>

REP::REP()
{
}

/**
 * Crea un archivo graphviz, escribe el código graphviz en él y luego llama al comando dot para crear
 * un archivo png
 * 
 * @param mbr La estructura MBR que se va a imprimir.
 */
void mbr_rep(MBR mbr, string pathgrafico, string pathDisco)
{

    string filename = pathDisco;
    size_t lastindex = filename.find_last_of(".");
    filename = filename.substr(0, lastindex) + ".dot";
    // creamos el graphviz
    FILE* graph;
    graph = fopen(filename.c_str(), "w");
    if (graph != NULL)
    {
        // string reporteEBR
        string reporteEBR = "";
        fprintf(graph, "digraph G {\n");
        fprintf(graph, "parent [\n");
        fprintf(graph, "shape=plaintext\n");
        fprintf(graph, "label=<<table border='1' cellborder='1'>\n");
        fprintf(graph, "<tr><td  colspan=\"3\" bgcolor='#C27664'>REPORTE DE MBR</td></tr>\n");
        fprintf(graph, "<tr><td port='tamanio' bgcolor='#E4C988'>tamano</td><td port='size' bgcolor='#E4C988'>%d</td></tr>\n", mbr.mbr_tamano);
        // Fecha y hora
        char fecha[100];
        strftime(fecha, 100, "%d/%m/%Y %H:%M:%S", localtime(&mbr.mbr_fecha_creacion));

        fprintf(graph, "<tr><td port='fecha' bgcolor='#E4C988'>fecha_creación</td><td port='size' bgcolor='#E4C988'>%s</td></tr>\n", fecha);
        fprintf(graph, "<tr><td port='dsk' bgcolor='#E4C988'>disk_signature</td><td port='size' bgcolor='#E4C988'>%d</td></tr>\n", mbr.mbr_disk_signature);
        for (int i = 0; i < 4; i++)
        {
            if (mbr.getPartition(i).part_name[0] != '\0')
            {
                fprintf(graph, "<tr><td colspan=\"3\" bgcolor='#CBE4DE'>PARTICIÓN</td></tr>\n");
                fprintf(graph, "<tr><td port='status' bgcolor='#4E6E81'>partition_status</td><td port='size' bgcolor='#4E6E81'>%c</td></tr>\n", mbr.getPartition(i).part_status);
                fprintf(graph, "<tr><td port='type' bgcolor='#F9DBBB'>partition_type</td><td port='size' bgcolor='#F9DBBB'>%c</td></tr>\n", mbr.getPartition(i).part_type);
                fprintf(graph, "<tr><td port='fit' bgcolor='#4E6E81'>partition_fit</td><td port='size' bgcolor='#4E6E81'>%c</td></tr>\n", mbr.getPartition(i).part_fit);
                fprintf(graph, "<tr><td port='start' bgcolor='#F9DBBB'>partition_start</td><td port='size' bgcolor='#F9DBBB'>%d</td></tr>\n", mbr.getPartition(i).part_start);
                fprintf(graph, "<tr><td port='s' bgcolor='#4E6E81'>partition_s</td><td port='size' bgcolor='#4E6E81'>%d</td></tr>\n", mbr.getPartition(i).part_size);
                fprintf(graph, "<tr><td port='name' bgcolor='#F9DBBB'>partition_name</td><td port='size' bgcolor='#F9DBBB'>%s</td></tr>\n", mbr.getPartition(i).part_name);
                if (mbr.getPartition(i).part_type == 'E')
                {
                    // recorremos las particiones lógicas
                    EBR ebr;
                    FILE* disco;
                    // variable para reporte de ebr
                    reporteEBR = "label=<<table border='1' cellborder='1'>\n";
                    reporteEBR += "<tr><td colspan=\"3\" bgcolor='#867070'>REPORTE EBR</td></tr>\n";
                    disco = fopen(pathDisco.c_str(), "rb+");
                    if (disco != NULL)
                    {
                        int lastEBR = mbr.getPartition(i).part_start;
                        
                        while (true)
                        {
                            fseek(disco, lastEBR, SEEK_SET);
                            fread(&ebr, sizeof(EBR), 1, disco);
                            if (ebr.part_name[0] != '\0')
                            {
                                fprintf(graph, "<tr><td colspan=\"3\" bgcolor='#0E8388'>PARTICIÓN LOGICA</td></tr>\n");
                                fprintf(graph, "<tr><td port='status' bgcolor='#F9F54B'>partition_status</td><td port='size' bgcolor='#F9F54B'>%c</td></tr>\n", ebr.part_status);
                                fprintf(graph, "<tr><td port='fit' bgcolor='#8BF5FA'>partition_fit</td><td port='size' bgcolor='#8BF5FA'>%c</td></tr>\n", ebr.part_fit);
                                fprintf(graph, "<tr><td port='start' bgcolor='#F9F54B'>partition_start</td><td port='size' bgcolor='#F9F54B'>%d</td></tr>\n", ebr.part_start);
                                fprintf(graph, "<tr><td port='s' bgcolor='#8BF5FA'>partition_s</td><td port='size' bgcolor='#8BF5FA'>%d</td></tr>\n", ebr.part_size);
                                fprintf(graph, "<tr><td port='name' bgcolor='#F9F54B'>partition_name</td><td port='size' bgcolor='#F9F54B'>%s</td></tr>\n", ebr.part_name);
                                fprintf(graph, "<tr><td port='next' bgcolor='#8BF5FA'>partition_next</td><td port='size' bgcolor='#8BF5FA'>%d</td></tr>\n", ebr.part_next);
                                // reporte de ebr
                                char buffer[1000]; // Definimos un buffer para almacenar el resultado de sprintf
                                sprintf(buffer, "<tr><td colspan=\"3\" bgcolor='#D5B4B4'>PARTICIÓN LOGICA</td></tr>\n");
                                reporteEBR += buffer;
                                sprintf(buffer, "<tr><td port='status' bgcolor='#E4D0D0'>partition_status</td><td port='size' bgcolor='#E4D0D0'>%c</td></tr>\n", ebr.part_status);
                                reporteEBR += buffer;
                                sprintf(buffer, "<tr><td port='fit' bgcolor='#F5EBEB'>partition_fit</td><td port='size' bgcolor='#F5EBEB'>%c</td></tr>\n", ebr.part_fit);
                                reporteEBR += buffer;
                                sprintf(buffer, "<tr><td port='start' bgcolor='#E4D0D0'>partition_start</td><td port='size' bgcolor='#E4D0D0'>%d</td></tr>\n", ebr.part_start);
                                reporteEBR += buffer;
                                sprintf(buffer, "<tr><td port='s' bgcolor='#F5EBEB'>partition_s</td><td port='size' bgcolor='#F5EBEB'>%d</td></tr>\n", ebr.part_size);
                                reporteEBR += buffer;
                                sprintf(buffer, "<tr><td port='name' bgcolor='#E4D0D0'>partition_name</td><td port='size' bgcolor='#E4D0D0'>%s</td></tr>\n", ebr.part_name);
                                reporteEBR += buffer;
                                sprintf(buffer, "<tr><td port='next' bgcolor='#F5EBEB'>partition_next</td><td port='size' bgcolor='#F5EBEB'>%d</td></tr>\n", ebr.part_next);
                                reporteEBR += buffer;

                            }
                            if (ebr.part_size == -1 || ebr.part_size == 0)
                            {
                                break;
                            }
                            lastEBR = ebr.part_next;
                        }
                        fclose(disco);
                    }
                }
            }
        }
        fprintf(graph, "</table>>\n");
        fprintf(graph, "];\n");
        // reporte de ebr
        if (reporteEBR != "")
        {
            fprintf(graph, "parent2[\n");
            fprintf(graph, "shape=plaintext\n");
            fprintf(graph, "%s", reporteEBR.c_str());
            fprintf(graph, "</table>>\n");
            fprintf(graph, "];\n");

        }
        fprintf(graph, "}\n");
        fclose(graph);
    } else
    {
        cout << "Error al crear el archivo dot" << endl;
        return;
    }
    // Si la carpeta no existe, la creamos
    string comando1 = "mkdir -p " + pathgrafico.substr(0, pathgrafico.find_last_of("/"));
    system(comando1.c_str());
    
    // llamamos al comando dot
    cout << "Generando gráfico..." << endl;
    string comando;
    string extension = std::filesystem::path(pathgrafico).extension();
    if (extension == ".jpg") {
        comando = "dot -Tjpg " + filename + " -o " + pathgrafico;
    } else if (extension == ".png") {
        comando = "dot -Tpng " + filename + " -o " + pathgrafico;
    } else if (extension == ".pdf") {
        comando = "dot -Tpdf " + filename + " -o " + pathgrafico;
    } else {
        cout << "Error: formato de archivo no soportado." << endl;
        return;
    }
    system(comando.c_str());

}

/**
 * Devuelve el porcentaje de espacio libre en el disco
 * 
 * @param mbr El MBR del disco.
 * 
 * @return El porcentaje de espacio libre en el disco.
 */
int porcentajeEspacioLibre(MBR mbr, string path)
{
    int espacioLibre = 0;
    for (int i = 0; i < 4; i++)
    {
        if (mbr.getPartition(i).part_name[0] != '\0')
        {
            if (mbr.getPartition(i).part_type == 'E' || mbr.getPartition(i).part_type == 'e')
            {
                FILE *disco;
                disco = fopen(path.c_str(), "rb+");
                if (disco != NULL)
                {
                    EBR ebr;
                    fseek(disco, mbr.getPartition(i).part_start, SEEK_SET);
                    fread(&ebr, sizeof (EBR), 1, disco);
                    while (ebr.part_next != -1)
                    {
                        espacioLibre += ebr.part_size;
                        fseek(disco, ebr.part_next, SEEK_SET);
                        fread(&ebr, sizeof (EBR), 1, disco);
                    }
                    espacioLibre += ebr.part_size;
                    fclose(disco);
                }
            } else
            {
                espacioLibre += mbr.getPartition(i).part_size;
            }
        }
    }
    return (mbr.mbr_tamano - espacioLibre) * 100 / mbr.mbr_tamano;
}


/**
 * Calcula el porcentaje de espacio ocupado en el disco
 * 
 * @param mbr El MBR del disco.
 * @param espacioOcupado La cantidad de espacio ocupado por la partición.
 * 
 * @return El porcentaje de espacio ocupado por la partición.
 */
int porcentajeEspacioOcupado(MBR mbr,int espacioOcupado)
{
    return espacioOcupado * 100 / mbr.mbr_tamano;
}

/**
 * Devuelve el porcentaje de espacio utilizado por una partición lógica
 * 
 * @param mbr El MBR del disco.
 * @param name Nombre de la partición
 * @param path La ruta al disco
 * 
 * @return El porcentaje de espacio utilizado por la partición lógica.
 */
int porcentajeLogica(int espacioMBR, int espacioLogica)
{
    return espacioLogica * 100 / espacioMBR;
}



/**
 * Crea un archivo .dot, luego crea un archivo .png a partir del archivo .dot
 * 
 * @param mbr La estructura MBR
 * @param path La ruta al archivo que desea crear.
 * @param pathDisco La ruta al archivo del disco.
 */
void rep_disk(MBR mbr, string path, string pathDisco)
{
    string filename = pathDisco;
    size_t lastindex = filename.find_last_of(".");
    filename = filename.substr(0, lastindex) + ".dot";
    FILE* graph;
    graph = fopen(filename.c_str(), "w");
    if (graph != NULL)
    {

        std::string tmpEBR = "";
        int rowspan = 0;
        string nombreArchivo = pathDisco;
        size_t lastindex = nombreArchivo.find_last_of(".");
        nombreArchivo = nombreArchivo.substr(0, lastindex);
        lastindex = nombreArchivo.find_last_of("/");
        nombreArchivo = nombreArchivo.substr(lastindex + 1, nombreArchivo.length());

        fprintf(graph, "digraph G {\n");
        fprintf(graph, "labelloc = \"t\";\n");
        fprintf(graph, "label = \"%s\";\n",nombreArchivo.c_str());
        fprintf(graph, "parent [\n");
        fprintf(graph, "shape=plaintext\n");
        fprintf(graph, "label=<\n");
        fprintf(graph, "<table border='1' cellborder='1' >\n");

        fprintf(graph, "<tr><td rowspan=\"3\" bgcolor='#0E8388'>MBR</td>\n");
        fprintf(graph, "<td rowspan=\"3\" bgcolor='#0E8388'>Espacio libre <br />%d%% del disco</td>\n",(porcentajeEspacioLibre(mbr,pathDisco)));
        for (int i = 0; i < 4; i++)
        {
            if (mbr.getPartition(i).part_name[0] != '\0')
            {
                if (mbr.getPartition(i).part_type == 'P')
                {
                    fprintf(graph, "<td rowspan=\"3\" bgcolor='#0E8388'>PRIMARIA<br />%d%% del disco</td>\n",(porcentajeEspacioOcupado(mbr,mbr.getPartition(i).part_size)));
                } else if (mbr.getPartition(i).part_type == 'E')
                {
                    rowspan = 0;
                    tmpEBR = "";

                    EBR ebr;
                    FILE* disco;
                    disco = fopen(pathDisco.c_str(), "rb+");
                    if (disco != NULL)
                    {
                        
                        int lastEBR = mbr.getPartition(i).part_start;
                        while (true)
                        {
                            fseek(disco, lastEBR, SEEK_SET);
                            fread(&ebr, sizeof(EBR), 1, disco);
                            if (ebr.part_name[0] != '\0')
                            {
                                tmpEBR += "<td rowspan=\"2\" bgcolor='#0E8388'>EBR</td>\n";
                                tmpEBR += "<td rowspan=\"2\" bgcolor='#0E8388'>LÓGICO<br />" + std::to_string(porcentajeLogica(mbr.mbr_tamano, ebr.part_size)) + "% de disco</td>\n";

                                rowspan += 2;
                            }
                            if (ebr.part_size == -1 || ebr.part_size == 0)
                            {
                                break;
                            }
                            lastEBR = ebr.part_next;
                        }
                        fclose(disco);
                    }
                }
            }
        }

        if (tmpEBR != "")
        {
            
            fputs("<td rowspan=\"1\" colspan=\"", graph);
            fprintf(graph, "%d", rowspan);
            fputs("\" bgcolor='#0E8388'>EXTENDIDA<br />", graph);
            fputs("</td>\n", graph);
            fprintf(graph, "</tr>\n");
            fprintf(graph, "<tr>\n");
            fputs(tmpEBR.c_str(), graph);

        }

        fprintf(graph, "</tr>\n");
        fprintf(graph, "</table>\n");
        fprintf(graph, ">];\n");
        fprintf(graph, "}\n");

    }
    fclose(graph);
    string comando1 = "mkdir -p " + path.substr(0, path.find_last_of("/"));
    system(comando1.c_str());

    // llamamos al comando dot
    cout << "Generando gráfico..." << endl;
    string comando;
    string extension = std::filesystem::path(path).extension();
    if (extension == ".jpg") {
        comando = "dot -Tjpg " + filename + " -o " + path;
    } else if (extension == ".png") {
        comando = "dot -Tpng " + filename + " -o " + path;
    } else if (extension == ".pdf") {
        comando = "dot -Tpdf " + filename + " -o " + path;
    } else {
        cout << "Error: formato de archivo no soportado." << endl;
        return;
    }
    system(comando.c_str());

}

void rep_superbloque(string path, string pathDisco, SuperBloque sp)
{
    string filename = pathDisco;
    size_t lastindex = filename.find_last_of(".");
    filename = filename.substr(0, lastindex) + ".dot";
    FILE* graph;
    graph = fopen(filename.c_str(), "w");
    if (graph != NULL)
    {
        string nombreArchivo = pathDisco;
        size_t lastindex = nombreArchivo.find_last_of(".");
        nombreArchivo = nombreArchivo.substr(0, lastindex);
        lastindex = nombreArchivo.find_last_of("/");
        nombreArchivo = nombreArchivo.substr(lastindex + 1, nombreArchivo.length());

        fprintf(graph, "digraph G {\n");
        fprintf(graph, "labelloc = \"t\";\n");
        fprintf(graph, "parent [\n");
        fprintf(graph, "shape=plaintext\n");
        fprintf(graph, "label=<\n");
        fprintf(graph, "<table border='1' cellborder='1' >\n");

        fprintf(graph, "<tr><td  bgcolor='#EA5455'>Reporte de SUPERBLOQUE</td></tr>\n");
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_nombre_hd: %s</td></tr>\n",nombreArchivo.c_str());
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_filesystem_type: %d</td></tr>\n",sp.s_filesystem_type);
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_inodes_count: %d</td></tr>\n",sp.s_inodes_count);
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_blocks_count: %d</td></tr>\n",sp.s_blocks_count);
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_free_blocks_count: %d</td></tr>\n",sp.s_free_blocks_count);
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_free_inodes_count: %d</td></tr>\n",sp.s_free_inodes_count);
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_mtime: %s</td></tr>\n",ctime(&sp.s_mtime));
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_umtime: %s</td></tr>\n",ctime(&sp.s_umtime));
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_mnt_count: %d</td></tr>\n",sp.s_mnt_count);
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_magic: %d</td></tr>\n",sp.s_magic);
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_inode_size: %d</td></tr>\n",sp.s_inode_size);
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_block_size: %d</td></tr>\n",sp.s_block_size);
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_first_ino: %d</td></tr>\n",sp.s_first_ino);
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_first_blo: %d</td></tr>\n",sp.s_first_blo);
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_bm_inode_start: %d</td></tr>\n",sp.s_bm_inode_start);
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_bm_block_start: %d</td></tr>\n",sp.s_bm_block_start);
        fprintf(graph, "<tr><td  bgcolor='#E4DCCF'>sp_inode_start: %d</td></tr>\n",sp.s_inode_start);
        fprintf(graph, "<tr><td  bgcolor='#F9F5EB'>sp_block_start: %d</td></tr>\n",sp.s_block_start);

        fprintf(graph, "</table>\n");
        fprintf(graph, ">];\n");
        fprintf(graph, "}\n");
    }
    fclose(graph);
    // Si la carpeta no existe, la creamos
    string comando1 = "mkdir -p " + path.substr(0, path.find_last_of("/"));
    system(comando1.c_str());
    
    // llamamos al comando dot
    cout << "Generando gráfico..." << endl;
    string comando;
    string extension = std::filesystem::path(path).extension();
    if (extension == ".jpg") {
        comando = "dot -Tjpg " + filename + " -o " + path;
    } else if (extension == ".png") {
        comando = "dot -Tpng " + filename + " -o " + path;
    } else if (extension == ".pdf") {
        comando = "dot -Tpdf " + filename + " -o " + path;
    } else {
        cout << "Error: formato de archivo no soportado." << endl;
        return;
    }
    system(comando.c_str());

        
}

void REP::make_rep(REP* rep, ListaDobleMount* listaMount)
{
    
    FILE* disco;
    disco = fopen(listaMount->buscar(rep->id)->path.c_str(), "rb+");
    if(disco != NULL)
    {
        MBR mbr;
        fread(&mbr, sizeof(MBR), 1, disco);
        if(mbr.mbr_tamano > 0)
        {
            if(rep->name == "mbr")
            {
                mbr_rep(mbr, rep->path, listaMount->buscar(rep->id)->path);
            }
            else if(rep->name == "disk")
            {
                rep_disk(mbr, rep->path, listaMount->buscar(rep->id)->path);
            }
            else if (rep->name == "sp")
            {
                Partition particiones[4] = {mbr.mbr_partition_1, mbr.mbr_partition_2, mbr.mbr_partition_3, mbr.mbr_partition_4};
                for (int i = 0; i < 4; i++)
                {
                    if (strcmp(particiones[i].part_name, listaMount->buscar(rep->id)->name) == 0)
                    {
                        fseek(disco, particiones[i].part_start, SEEK_SET);
                        SuperBloque sp;
                        fread(&sp, sizeof(SuperBloque), 1, disco);
                        rep_superbloque(rep->path, listaMount->buscar(rep->id)->path, sp);
                        break;
                    }
                }
            }
        }
        else
        {
            cout << "El disco no existe" << endl;
        }
    }
    else
    {
        cout << "El disco no existe" << endl;
    }
    fclose(disco);
}