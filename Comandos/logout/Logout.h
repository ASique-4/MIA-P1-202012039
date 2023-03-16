#ifndef LOGOUT_H
#define LOGOUT_H

#include <iostream>
#include "../login/user.h"

using namespace std;


class Logout
{
    public:
        Logout();

        void make_logout(User *user);

    private:

};

#endif // LOGOUT_H