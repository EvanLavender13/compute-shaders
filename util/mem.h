#ifndef MEM_H_
#define MEM_H_

#include <stdlib.h>

static int nrefs;

void *
memalloc(int n)
{
    nrefs++;
    return malloc(n);
}

void
memfree(void *ptr)
{
    nrefs--;
    free(ptr);
}

#endif