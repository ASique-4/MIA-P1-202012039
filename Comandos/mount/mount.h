#ifndef MOUNT_H
#define MOUNT_H

#include <iostream>
#include "../mount/ListaDobleMount.h"

using namespace std;

class MOUNT 
{
    public:
        MOUNT();
        string path = "";
        char name[16] = "\0";
        ListaDobleMount* make_mount(MOUNT *disco);
    private:
};

#endif // MOUNT_H
