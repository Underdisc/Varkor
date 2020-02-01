#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "error.h"
#include "shader.h"
#include "texture.h"

void resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void Core()
{
    // window setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800;
    int height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "ggj", NULL, NULL);
    LogAbortIf(!window, "glfw window creation failed");

    glfwMakeContextCurrent(window);

    bool glad_loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LogAbortIf(!glad_loaded, "glad initialization failed");

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, resize_callback);

    // shader setup
    Shader solid("shader/solid.vs", "shader/solid.fs");

    // texture setup
    Texture texture_0("texture_1.png");

    // buffer setup
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f
    };

    unsigned int indicies[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*   )0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool active = true;
    while (active)
    {
        process_input(window);

        glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // matrix transformation setup
        const float pi = 3.1415926535897f;
        float rotation = pi * glfwGetTime();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, -rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        float dist = -3.0f;
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, dist));
        float fov = pi / 4.0f;
        glm::mat4 proj = glm::mat4(1.0f);
        proj = glm::perspective(fov, (float)width/(float)height, 0.1f, 100.0f);

        solid.Use();
        int loc = glGetUniformLocation(solid.Id(), "tex_0");
        glUniform1i(loc, 0);
        solid.SetMat4("model", glm::value_ptr(model));
        solid.SetMat4("view", glm::value_ptr(view));
        solid.SetMat4("proj", glm::value_ptr(proj));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_0.Id());
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        active = !glfwWindowShouldClose(window);
    }

    glfwTerminate();
}

int main(void)
{
    Error::Init("log.err");
    Core();
    Error::Purge();
    return 0;
} 