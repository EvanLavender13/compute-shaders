#ifndef APP_H_
#define APP_H_

#include "cell.h"
#include "mem.h"
#include "shader.h"
#include "texture.h"
#include "window.h"

typedef struct App App;
struct App
{
    char *name;
    int minimized;
    int running;

    GLFWwindow *windowhandle;

    Shader *shader;
    Cell   *root;
};

struct
{
    double currtime;
    double lasttime;
    double time;
} *delta;

void
deltaupdate()
{
    delta->lasttime = delta->currtime;
    delta->currtime = glfwGetTime();
    delta->time = delta->currtime - delta->lasttime;
}

struct
{
    int framecount;
    double currtime;
    double lasttime;
} *fps;

void
fpsupdate()
{
    fps->currtime = glfwGetTime();
    fps->framecount++;
    if (fps->currtime - fps->lasttime >= 1.0) {
        printf("%f ms/frame %d fps\n", 1000.0 / fps->framecount, fps->framecount);
        fps->framecount = 0;
        fps->lasttime++;
    }
}

static App *instance = NULL;

void
appclose()
{
    printf("Closing \"%s\"\n", instance->name);
    instance->running = 0;
}

void
appinitutils()
{
    GLuint framebufferid;
    Shader *shader;
    Cell *cell;

    shader = memalloc(sizeof(Shader));
    initshader(shader, "../shaders/test.glsl", GL_COMPUTE_SHADER);
    shader->textureid = inittexture_empty(800, 600);
    glGenFramebuffers(1, &framebufferid);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferid);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shader->textureid, 0);

    instance->shader = shader;
    shaderlink(instance->shader);

    cell = memalloc(sizeof(Cell));
    initcell(cell, 1, 0, 0, 800, 600);
    cell->shader = shader;
    instance->root = cell;
}

void
initapp(char *name)
{
    printf("Initializing \"%s\"\n", name);
    instance = memalloc(sizeof(App));
    instance->windowhandle = initwindow(800, 600, name);
    instance->name = name;
    instance->minimized = 0;
    instance->running   = 1;
    
    delta = memalloc(sizeof(*delta));
    fps   = memalloc(sizeof(*fps));
    fps->lasttime   = glfwGetTime();
    fps->currtime   = glfwGetTime();
    fps->framecount = 0;

    appinitutils();

    glfwSetWindowCloseCallback(instance->windowhandle, appclose);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);
}

void
apprun()
{
    GLenum status;

    glUniform1i(instance->shader->textureid, 0);

    cellsplit(instance->root);
    cellsplit(instance->root->cells);
    cellsplit(instance->root->cells->cells);
    cellsplit(instance->root->cells->cells->cells);

    while (instance->running) {
        deltaupdate();
        fpsupdate();

        if (instance->minimized == 0) {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            cellupdate(instance->root);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_LINEAR);
            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                printf("what!");
            }

            glUseProgram(0);
        }

        windowupdate(instance->windowhandle);
    }
}

void
appdelete()
{
    windowdelete(instance->windowhandle);
    shaderdelete(instance->shader);
    celldelete(instance->root);

    memfree(instance);
    memfree(delta);
    memfree(fps);
}

#endif