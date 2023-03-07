#ifndef PARTICIONMOUNT_H
#define PARTICIONMOUNT_H

#include <iostream>
#include <string.h>

using namespace std;

class ParticionMount
{
    public:
        string id;
        char name[16];
        string path;
        ParticionMount *siguiente = NULL;
        ParticionMount *anterior = NULL;
    private:
};

#endif // PARTICIONMOUNT_H