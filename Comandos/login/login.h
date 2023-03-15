#ifndef LOGIN_H
#define LOGIN_H

#include <iostream>

using namespace std;

class Login
{
    public:
        Login();
        string user;
        string password;
        string id;

        void make_login(Login *login, ListaDobleMount *listaDobleMount);

    private:

};

#endif // LOGIN_H