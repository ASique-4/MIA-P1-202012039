#ifndef RMUSR_H
#define RMUSR_H

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

class Rmusr
{
    public:
        Rmusr();
        char user[10];
        void ejecutar(string path, int start, Rmusr *rmusr);
    private:
};

#endif // RMUSR_H