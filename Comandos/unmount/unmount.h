#ifndef UNMOUNT_H
#define UNMOUNT_H

#include <iostream>

using namespace std;

class UNMOUNT
{
    public:
        UNMOUNT();
        char name[16] = "\0";
        void make_unmount(UNMOUNT *disco);
    private:
};

#endif // UNMOUNT_H