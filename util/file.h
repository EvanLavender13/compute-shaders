#ifndef FILE_H_
#define FILE_H_

#include "stdio.h"

#define UNUSED(x) (void)(x)

int
filesize(FILE *file)
{
    int size;

    fseek(file, 0l, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

#endif