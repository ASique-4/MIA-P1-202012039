#ifndef MOUNT_H
#define MOUNT_H

#include <iostream>

using namespace std;

class MOUNT 
{
    public:
        MOUNT();
        string path = "";
        char name[16] = "\0";
        void make_mount(MOUNT *disco);
    private:
};

#endif // MOUNT_H
