#ifndef LOGIN_H
#define LOGIN_H

#include <iostream>
#include "user.h"

using namespace std;

User::User()
{
    id = "";
    tipo = "";
    grupo = "";
    usuario = "";
    password = "";
}

class Login
{
    public:
        Login();
        string user;
        string password;
        string id;

        User* make_login(Login *login, ListaDobleMount *listaDobleMount);

    private:

};

#endif // LOGIN_H