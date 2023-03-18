#ifndef MKUSR_H
#define MKUSR_H

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

class Mkusr
{
    public:
        Mkusr();
        char user[10];
        char pass[10];
        char grp[10];
        void ejecutar(string path, int start, Mkusr *mkusr);
    private:
};

#endif // MKUSR_H