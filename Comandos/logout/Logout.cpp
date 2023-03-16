#include "Logout.h"
#include <iostream>

using namespace std;

Logout::Logout()
{
}

void Logout::make_logout(User *user)
{
    if (user->usuario != "")
    {
        user->id = "";
        user->tipo = "";
        user->grupo = "";
        user->usuario = "";
        user->password = "";
        cout << "Se ha cerrado la sesion" << endl;
    }
    else
    {
        cout << "No hay ninguna sesion iniciada" << endl;
    }
}