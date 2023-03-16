#include <stdio.h>
#include <fstream>
#include <iomanip> // Para setw

#include "./Analizador/Analizador.h"


#include "./Comandos/mkdisk/mkdisk.cpp"
#include "./Comandos/rmdisk/rmdisk.cpp"
#include "./Comandos/fdisk/fdisk.cpp"
#include "./Comandos/unmount/unmount.cpp"
#include "./Comandos/mount/mount.cpp"
#include "./Comandos/rep/rep.cpp"
#include "./Comandos/mkfs/mkfs.cpp"
#include "./Comandos/execute/execute.cpp"
#include "./Comandos/login/login.cpp"
#include "./Comandos/logout/Logout.cpp"

using namespace std;

int main()
{
    string titulo = "Proyecto 1 - MIA";
    string nombre = "Angel Francisco Sique Santos";
    string codigo = "202012039";
    string mensaje = "Ingrese el comando a analizar";

    int ancho = 40;
    cout << "+" << setfill('-') << setw(ancho-2) << "+" << endl;
    cout << "|" << setfill(' ') << setw(ancho-2) << "|" << endl;
    cout << "|" << setfill(' ') << setw((ancho - titulo.length()) / 2 + titulo.length()-1) << titulo << setw(((ancho - titulo.length()) / 2) - 1) << "|" << endl;
    cout << "|" << setfill(' ') << setw((ancho - nombre.length()) / 2 + nombre.length()-1) << nombre << setw(((ancho - nombre.length()) / 2) - 1) << "|" << endl;
    cout << "|" << setfill(' ') << setw((ancho - codigo.length()) / 2 + codigo.length()-1) << codigo << setw(((ancho - codigo.length()) / 2)) << "|" << endl;
    cout << "|" << setfill(' ') << setw(ancho-2) << "|" << endl;
    cout << "+" << setfill('-') << setw(ancho-2) << "+" << endl;
    cout << endl;

    cout << mensaje << endl;

  char comando[400];
  bool repetir = true;
  do{
    //Pedimos el comando
    cout<< "~ ";
    scanf(" %[^\n]",comando);
    //Ahora analizamos
    Analizar(comando);
  } while(repetir);
  return 0;
}