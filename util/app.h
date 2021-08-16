#ifndef APP_H_
#define APP_H_

#include "agent.h"
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

    Shader *cellshader;
    Shader *blurshader;
    Shader *coolshader;

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
        // printf("delta %f\n", delta->time);
        fps->framecount = 0;
        fps->lasttime++;
    }
}

static App *instance = NULL;
static int numagents = 1024 * 25;

void
appclose()
{
    printf("Closing \"%s\"\n", instance->name);
    instance->running = 0;
}

void
_cellshader(Shader *cellshader)
{
    GLuint framebufferid;

    initshader(cellshader, "../shaders/cell.glsl", GL_COMPUTE_SHADER);
    cellshader->textureid = inittexture_empty(800, 600);
    glGenFramebuffers(1, &framebufferid);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferid);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cellshader->textureid, 0);

    instance->cellshader = cellshader;
    shaderlink(instance->cellshader);
}

void
_blurshader(Shader *blurshader)
{
    initshader(blurshader, "../shaders/blur.glsl", GL_COMPUTE_SHADER);

    instance->blurshader = blurshader;
    shaderlink(instance->blurshader);
}

void
_coolshader(Shader *coolshader)
{
    initshader(coolshader, "../shaders/cool.glsl", GL_COMPUTE_SHADER);

    instance->coolshader = coolshader;
    shaderlink(instance->coolshader);
}

void
appinitutils()
{
    int i;
    GLuint storageid;
    Shader *cellshader;
    Shader *blurshader;
    Shader *coolshader;
    Cell *cell;
    Agent *agents;

    cellshader = memalloc(sizeof(Shader));
    _cellshader(cellshader);

    blurshader = memalloc(sizeof(Shader));
    _blurshader(blurshader);

    coolshader = memalloc(sizeof(Shader));
    _coolshader(coolshader);

    agents = memalloc(sizeof(Agent) * numagents);
    for (i = 0; i < numagents; i++) {
        agents[i].x = 400;
        agents[i].y = 300;
        agents[i].angle = i + 1;
    }

    glGenBuffers(1, &storageid);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageid);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Agent) * numagents, agents, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, storageid);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    coolshader->storage = storageid;

    cell = memalloc(sizeof(Cell));
    initcell(cell, 1, 0, 0, 800, 600);
    cell->shader = cellshader;
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
_blur()
{
    glUseProgram(instance->blurshader->progid);
    glUniform1f(instance->blurshader->udelta, delta->time);
    glUniform1i(instance->blurshader->uwidth,  800);
    glUniform1i(instance->blurshader->uheight, 600);
    glDispatchCompute(80, 60, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void
apprun()
{
    GLenum status;

    glUniform1i(instance->cellshader->textureid, 0);

    cellsplit(instance->root);
    cellsplit(instance->root->botleft);
    cellsplit(instance->root->topleft);
    cellsplit(instance->root->topright);
    cellsplit(instance->root->botright);

    cellsplit(instance->root->botleft->topright);
    cellsplit((instance->root->topleft)->botright);
    cellsplit((instance->root->topright)->botleft);
    cellsplit((instance->root->botright)->topleft);

    cellsplit((instance->root->botleft->topright)->topright);
    cellsplit(((instance->root->topleft)->botright)->botright);
    cellsplit(((instance->root->topright)->botleft)->botleft);
    cellsplit(((instance->root->botright)->topleft)->topleft);

    while (instance->running) {
        deltaupdate();
        fpsupdate();

        if (instance->minimized == 0) {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // update cells
            cellupdate(instance->root);

            _blur();

            // move agents
            glUseProgram(instance->coolshader->progid);
            glUniform1f(instance->coolshader->udelta, delta->time);
            glUniform1i(instance->coolshader->uwidth,  800);
            glUniform1i(instance->coolshader->uheight, 600);
            glDispatchCompute(numagents / 16, 10, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

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
    shaderdelete(instance->cellshader);
    shaderdelete(instance->blurshader);
    shaderdelete(instance->coolshader);
    celldelete(instance->root);

    memfree(instance);
    memfree(delta);
    memfree(fps);
}

#endif