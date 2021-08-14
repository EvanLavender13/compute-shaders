#include "stdio.h"

#define GLEW_STATIC
#include "GL/glew.h"

#include "app.h"

void
error()
{
    GLenum err;
    for (;;) {
        err = glGetError();
        if (err == GL_NO_ERROR) break;
        printf("ERROR: %d\n", err);
    }
}

int
main()
{
    printf("Hello\n");

    initapp("App");
    apprun();
    appdelete();

    error();
    printf("References remaining: %d\n", nrefs);
    return 0;
}