#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(int argc, const char** argv)
{
    if (!glfwInit())
    {
        printf("Failed to initialise GLFW!\n");
        return 1;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    GLFWwindow *window = glfwCreateWindow(
        640, 480,
        "Super Tic-Tac-Toe",
        NULL,
        NULL
    );
    if (!window)
    {
        printf("Failed to create window!\n");
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialise OpenGL context!\n");
        return 1;
    }

    glClearColor(0.5f, 0.f, 1.f, 1.f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
