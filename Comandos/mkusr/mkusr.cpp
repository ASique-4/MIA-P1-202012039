#include "mkusr.h"


using namespace std;

Mkusr::Mkusr()
{
}

void Mkusr::ejecutar(string path, int start, Mkusr *mkusr)
{
    FILE *archivo;
    archivo = fopen(path.c_str(), "r+b");
    if (archivo == NULL)
    {
        cout << "No se pudo abrir el archivo" << endl;
        return;
    }

    // Buscamos el último grupo o usuario y validamos que el usuario no exista ya
    int ultimo_grupo_o_usuario = -1;  // Inicializamos en -1 para detectar si no se encontró ningún grupo o usuario
    char linea[100];
    int pos = 0;
    bool usuario_existente = false;
    fseek(archivo, start, SEEK_SET);
    while (fgets(linea, 100, archivo) != NULL)
    {
        string linea_str(linea);
        if (linea_str.find(",U,") != string::npos)
        {
            char* token = strtok(linea, ",");
            int numero_grupo_o_usuario = atoi(token);
            if (numero_grupo_o_usuario > ultimo_grupo_o_usuario)
            {
                ultimo_grupo_o_usuario = numero_grupo_o_usuario;
            }
            pos = ftell(archivo);
        }
        if (linea_str.find(",G,") != string::npos)
        {
            pos = ftell(archivo);
        }
        if (linea[0] == '\0')
        {
            break;
        }
    }

    // Validamos que el grupo exista
    bool grupo_existente = false;
    fseek(archivo, start, SEEK_SET);
    while (fgets(linea, 100, archivo) != NULL)
    {
        string linea_str(linea);
        if (linea_str.find(",G,") != string::npos)
        {
            string grupo_linea = linea_str.substr(linea_str.find_first_of(",G,")+3, linea_str.length()-1);
            // Le quitamos el salto de línea y espacios
            grupo_linea.erase(grupo_linea.find_last_not_of(" \n\r\t")+1);
            if (grupo_linea == mkusr->grp) {
                grupo_existente = true;
                break;
            }
        }
    }

    if (usuario_existente) {
        cout << "El usuario ya existe" << endl;
    }
    else if (!grupo_existente) {
        cout << "El grupo no existe" << endl;
    }
    else {
        // Creamos la cadena del nuevo usuario
        int nuevo_usuario = ultimo_grupo_o_usuario + 1;
        string usuario = to_string(nuevo_usuario) + ",U," + mkusr->user + "," + mkusr->pass + "," + mkusr->grp + "\n";
        
        // Escribimos la cadena en el archivo después del último grupo o usuario
        fseek(archivo, pos, SEEK_SET);
        fwrite(usuario.c_str(), sizeof(char), usuario.length(), archivo);
        cout << "Se ha creado el usuario " << mkusr->user << " con el número " << nuevo_usuario << endl;
    }

    fclose(archivo);
}



