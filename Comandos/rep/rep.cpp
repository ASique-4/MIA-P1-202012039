#include "rep.h"
#include "../fdisk/ebr.h"
#include "../mount/ListaDobleMount.h"
#include "../Estructuras/SuperBloque.h"
#include <time.h>
#include <iostream>
#include <fstream>

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
    cout << "MBR" << endl;

    string filename = pathDisco;
    size_t lastindex = filename.find_last_of(".");
    filename = filename.substr(0, lastindex) + ".dot";
    // creamos el graphviz
    FILE* graph;
    graph = fopen(filename.c_str(), "w");
    if (graph != NULL)
    {
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
                    disco = fopen(pathDisco.c_str(), "rb+");
                    if (disco != NULL)
                    {
                        fseek(disco, sizeof(MBR), SEEK_SET);
                        while (true)
                        {
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
                            }
                            if (ebr.part_size == -1 || ebr.part_size == 0)
                            {
                                break;
                            }
                            fseek(disco, ebr.part_next, SEEK_SET);
                        }
                        fclose(disco);
                    }
                }
            }
        }
        fprintf(graph, "</table>>\n");
        fprintf(graph, "];\n");
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
    cout << "dot -Tjpg " << filename << " -o " << pathgrafico << endl;
    string comando = "dot -Tjpg " + filename + " -o " + pathgrafico;
    system(comando.c_str());
}

/**
 * Devuelve el porcentaje de espacio libre en el disco
 * 
 * @param mbr El MBR del disco.
 * 
 * @return El porcentaje de espacio libre en el disco.
 */
int porcentajeEspacioLibre(MBR mbr)
{
    int espacioLibre = 0;
    for (int i = 0; i < 4; i++)
    {
        if (mbr.getPartition(i).part_name[0] != '\0')
        {
            espacioLibre += mbr.getPartition(i).part_size;
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
int porcentajeLogica(MBR mbr, string name, string path)
{
    int espacioLogica = 0;
    for (int i = 0; i < 4; i++)
    {
        if (mbr.getPartition(i).part_name[0] != '\0')
        {
            if (mbr.getPartition(i).part_type == 'E')
            {
                EBR ebr;
                FILE* disco;
                disco = fopen(path.c_str(), "rb+");
                if (disco != NULL)
                {
                    int lastEBR = sizeof(MBR);
                    while (true)
                    {
                        fseek(disco, lastEBR, SEEK_SET);
                        fread(&ebr, sizeof(EBR), 1, disco);
                        if (ebr.part_name != name)
                        {
                            espacioLogica = ebr.part_size;
                            break;
                        }
                        lastEBR += sizeof(EBR);
                    }
                    fclose(disco);
                }
            }
        }
    }
    return espacioLogica * 100 / mbr.mbr_tamano;
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
    cout << "mbr_tamano: " << mbr.mbr_tamano << endl;
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
        fprintf(graph, "label = \"%s\";\n",nombreArchivo.c_str());
        fprintf(graph, "parent [\n");
        fprintf(graph, "shape=plaintext\n");
        fprintf(graph, "label=<\n");
        fprintf(graph, "<table border='1' cellborder='1' >\n");

        fprintf(graph, "<tr><td rowspan=\"3\" bgcolor='#0E8388'>MBR</td>\n");
        fprintf(graph, "<td rowspan=\"3\" bgcolor='#0E8388'>Espacio libre <br />%d%% del disco</td>\n",(porcentajeEspacioLibre(mbr)));
        for (int i = 0; i < 4; i++)
        {
            if (mbr.getPartition(i).part_name[0] != '\0')
            {
                if (mbr.getPartition(i).part_type == 'P')
                {
                    fprintf(graph, "<td rowspan=\"3\" bgcolor='#0E8388'>PRIMARIA<br />%d%% del disco</td>\n",(porcentajeEspacioOcupado(mbr,mbr.getPartition(i).part_size)));
                } else if (mbr.getPartition(i).part_type == 'E')
                {
                    int rowspan = 0;
                    std::string tmpEBR = "";

                    EBR ebr;
                    FILE* disco;
                    disco = fopen(pathDisco.c_str(), "rb+");
                    if (disco != NULL)
                    {
                        
                        int lastEBR = sizeof(MBR);
                        while (true)
                        {
                            fseek(disco, lastEBR, SEEK_SET);
                            fread(&ebr, sizeof(EBR), 1, disco);
                            if (ebr.part_name[0] != '\0')
                            {
                                tmpEBR += "<td rowspan=\"2\" bgcolor='#0E8388'>EBR</td>\n";
                                tmpEBR += "<td rowspan=\"2\" bgcolor='#0E8388'>LÓGICO<br />" + std::to_string(porcentajeLogica(mbr,mbr.getPartition(i).part_name,pathDisco)) + "% de disco</td>\n";

                                rowspan += 2;
                            }
                            if (ebr.part_size == -1 || ebr.part_size == 0)
                            {
                                break;
                            }
                            lastEBR += sizeof(EBR);
                        }
                        fclose(disco);
                    }
                    fputs("<td rowspan=\"1\" colspan=\"", graph);
                    fprintf(graph, "%d", rowspan);
                    fputs("\" bgcolor='#0E8388'>EXTENDIDA<br />", graph);
                    fputs("</td>\n", graph);
                    fprintf(graph, "</tr>\n");
                    fprintf(graph, "<tr>\n");
                    fputs(tmpEBR.c_str(), graph);

                } else if (mbr.getPartition(i).part_type == 'L')
                {
                    fprintf(graph, "<td rowspan=\"3\" bgcolor='#0E8388'>LOGICA <br />%d%%</td>\n",(porcentajeEspacioOcupado(mbr,mbr.getPartition(i).part_size)));
                }
            }
        }

        fprintf(graph, "</tr>\n");
        fprintf(graph, "</table>\n");
        fprintf(graph, ">];\n");
        fprintf(graph, "}\n");

    }
    fclose(graph);
    string comando1 = "mkdir -p " + path.substr(0, path.find_last_of("/"));
    system(comando1.c_str());

    string comando = "dot -Tpng " + filename + " -o " + path;
    system(comando.c_str());

}

void rep_superbloque(SuperBloque sp, string path, string pathDisco)
{
    cout << "SUPERBLOQUE" << endl;
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

        fprintf(graph, "<tr><td rowspan=\"3\" bgcolor='#0E8388'>Reporte de SUPERBLOQUE</td>\n");
        fprintf(graph, "<td rowspan=\"3\" bgcolor='#0E8388'>sb_nombre_hd: %s</td>\n",nombreArchivo.c_str());
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_filesystem_type: %d</td>\n",sp.s_filesystem_type);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_inodes_count: %d</td>\n",sp.s_inodes_count);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_blocks_count: %d</td>\n",sp.s_blocks_count);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_free_blocks_count: %d</td>\n",sp.s_free_blocks_count);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_free_inodes_count: %d</td>\n",sp.s_free_inodes_count);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_mtime: %s</td>\n",ctime(&sp.s_mtime));
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_umtime: %s</td>\n",ctime(&sp.s_umtime));
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_mnt_count: %d</td>\n",sp.s_mnt_count);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_magic: %d</td>\n",sp.s_magic);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_inode_size: %d</td>\n",sp.s_inode_size);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_block_size: %d</td>\n",sp.s_block_size);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_first_ino: %d</td>\n",sp.s_first_ino);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_first_blo: %d</td>\n",sp.s_first_blo);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_bm_inode_start: %d</td>\n",sp.s_bm_inode_start);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_bm_block_start: %d</td>\n",sp.s_bm_block_start);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_inode_start: %d</td>\n",sp.s_inode_start);
        fprintf(graph, "< rowspan=\"3\" bgcolor='#0E8388'>sb_block_start: %d</td>\n",sp.s_block_start);
        fprintf(graph, "</tr>\n");

        fprintf(graph, "</table>\n");
        fprintf(graph, ">];\n");
        fprintf(graph, "}\n");
    }
    fclose(graph);
    string comando1 = "mkdir -p " + path.substr(0, path.find_last_of("/"));
    system(comando1.c_str());

    string comando = "dot -Tpng " + filename + " -o " + path;
    system(comando.c_str());
        
}

void REP::make_rep(REP* rep, ListaDobleMount* listaMount)
{
    
    FILE* disco;
    cout << "path: " << listaMount->buscar(rep->id)->path << endl;
    disco = fopen(listaMount->buscar(rep->id)->path.c_str(), "rb+");
    if(disco != NULL)
    {
        MBR mbr;
        fread(&mbr, sizeof(MBR), 1, disco);
        if(mbr.mbr_tamano > 0)
        {
            if(rep->name == "mbr")
            {
                cout << "MBR" << endl;
                mbr_rep(mbr, rep->path, listaMount->buscar(rep->id)->path);
            }
            else if(rep->name == "disk")
            {
                cout << "DISK" << endl;
                rep_disk(mbr, rep->path, listaMount->buscar(rep->id)->path);
            }
            else if (rep->name == "sp")
            {
                cout << "SUPERBLOQUE" << endl;
                //rep_superbloque();
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