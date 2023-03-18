#ifndef MKGRP_H
#define MKGRP_H

#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

class MKGRP
{
public:
    MKGRP();
    void ejecutarMKGRP(string id, string path, int start);
};

#endif // MKGRP_H