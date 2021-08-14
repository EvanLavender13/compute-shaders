#ifndef SHADER_H_
#define SHADER_H_

#include "errno.h"
#include "stdio.h"

#include "GL/glew.h"

#include "file.h"
#include "mem.h"

typedef struct Shader Shader;
struct Shader
{
    int progid;
    union {
        int compid; // compute 
    };

    int textureid;
    int framebufferid;

    int utexture;
    int udelta;
    int uwidth;
    int uheight;
    int ux1, uy1;
    int ux2, uy2;
};

int
shadercompile(const char *src, int type)
{
    int shader;

    shader = glCreateShader(type);
    // TODO: error check
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

void
initshader(Shader *shader, char *filepath, int type)
{
    char error[4096];
    char *src;
    int size, handle, status;
    FILE *file;

    // load shader source from file
    file = fopen(filepath, "r");
    if (file == NULL) {
        printf("unable to open file: %s, errno: %d\n", filepath, errno);
        return;
    }

    size = filesize(file);
    src = memalloc(size + 1);
    fread(src, 1, size, file);
    src[size] = 0; // null terminated
    fclose(file);

    // compile shader from source
    handle = shadercompile(src, type);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(handle, sizeof(error), NULL, error);
        printf("shader compile error in %s: %s\n", filepath, error);
    }

    memfree(src);

    // create shader program
    shader->progid = glCreateProgram();
    shader->compid = handle;
    glAttachShader(shader->progid, shader->compid);
}

void
shaderlink(Shader *shader)
{
    char error[4096];
    int status;

    // link
    glLinkProgram(shader->progid);
    glGetProgramiv(shader->progid, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        glGetProgramInfoLog(shader->progid, sizeof(error), NULL, error);
        printf("shader link error: %s\n", error);
    }

    shader->utexture = glGetUniformLocation(shader->progid, "utexture");
    shader->udelta   = glGetUniformLocation(shader->progid, "udelta");
    shader->uwidth   = glGetUniformLocation(shader->progid, "uwidth");
    shader->uheight  = glGetUniformLocation(shader->progid, "uheight");
    shader->ux1      = glGetUniformLocation(shader->progid, "ux1");
    shader->uy1      = glGetUniformLocation(shader->progid, "uy1");
    shader->ux2      = glGetUniformLocation(shader->progid, "ux2");
    shader->uy2      = glGetUniformLocation(shader->progid, "uy2");

    // detach and validate
    glDetachShader(shader->progid, shader->compid);
    glValidateProgram(shader->progid);
    glGetProgramiv(shader->progid, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetProgramInfoLog(shader->progid, sizeof(error), NULL, error);
        printf("shader validation error: %s\n", error);
    }
}

void
shaderdelete(Shader *shader)
{
    glDeleteProgram(shader->progid);
    memfree(shader);
}

#endif