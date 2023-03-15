#ifndef BLOQUEDECARPETAS_H
#define BLOQUEDECARPETAS_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "Content.h"

using namespace std;

Content::Content()
{
}

class BloqueDeCarpetas
{
    public:
        BloqueDeCarpetas();
        Content b_content[4];
};

#endif // BLOQUEDECARPETAS_H