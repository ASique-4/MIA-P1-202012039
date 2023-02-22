#include <stdio.h>
#include <fstream>
#include <iomanip> // Para setw

#include "./Analizador/Analizador.h"

#include "./Comandos/mkdisk/mkdisk.cpp"
#include "./Comandos/rmdisk/rmdisk.cpp"

using namespace std;

int main()
{
    string titulo = "Proyecto 1 - MIA";
    string nombre = "Angel Francisco Sique Santos";
    string codigo = "202012039";
    string mensaje = "Ingrese el comando a analizar";

    int ancho = 40;
    cout << setfill('=') << setw(ancho) << "" << endl;
    cout << setfill(' ') << setw((ancho - titulo.length()) / 2 + titulo.length()) << titulo << endl;
    cout << setfill('=') << setw(ancho) << "" << endl;
    cout << setfill(' ') << setw((ancho - nombre.length()) / 2 + nombre.length()) << nombre << endl;
    cout << setfill('=') << setw(ancho) << "" << endl;
    cout << setfill(' ') << setw((ancho - codigo.length()) / 2 + codigo.length()) << codigo << endl;
    cout << setfill('=') << setw(ancho) << "" << endl;
    cout << endl;

    cout << mensaje << endl;

  char comando[400];
  bool repetir = true;
  do{
    //Pedimos el comando
    cout<< "> ";
    scanf(" %[^\n]",comando);
    //Ahora analizamos
    Analizar(comando);
  } while(repetir);
  return 0;
}