#ifndef REP_H
#define REP_H

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

class REP
{
    public:
        REP();
        string name = "";
        string path = "";
        string id = "";
        string ruta = "";
        void make_rep(REP* rep, ListaDobleMount* listaMount);
};

#endif // REP_H