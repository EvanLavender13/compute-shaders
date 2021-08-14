#ifndef WINDOW_H_
#define WINDOW_H_

#include "GLFW/glfw3.h"
#include "GL/glew.h"

void
errorcallback(int err, const char *desc)
{
    printf("GLFW Error (%d): %s\n", err, desc);
}

void
windowvsync(int v)
{
    glfwSwapInterval(v);
}

GLFWwindow *
initwindow(int width, int height, char *name)
{
    int success;
    GLFWwindow *handle;

    success = glfwInit();
    if (success == 0) return NULL;
    glfwSetErrorCallback(errorcallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   /* TODO: make resizable */
    glfwWindowHint(GLFW_SAMPLES, 4);            /* TODO: yuh??? */

    handle = glfwCreateWindow(width, height, name, NULL, NULL);
    glfwMakeContextCurrent(handle);
    glewExperimental = GL_TRUE;
    glewInit();

    printf("OpenGL Info:\n");
    printf("  Vendor: %s\n", glGetString(GL_VENDOR));
    printf("  Renderer: %s\n", glGetString(GL_RENDERER));
    printf("  Version: %s\n", glGetString(GL_VERSION));

    windowvsync(1);

    return handle;
}

void
windowupdate(GLFWwindow *handle)
{
    glfwPollEvents();
    glfwSwapBuffers(handle);
}

void
windowdelete(GLFWwindow *handle)
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

#endif