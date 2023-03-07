#ifndef UNMOUNT_H
#define UNMOUNT_H

#include <iostream>

using namespace std;

class UNMOUNT
{
    public:
        UNMOUNT();
        string id = "";
        void make_unmount(UNMOUNT *disco, ListaDobleMount *lista);
    private:
};

#endif // UNMOUNT_H