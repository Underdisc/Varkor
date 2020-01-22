#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "error.h"

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

unsigned int compile_shader(const char* shader_source, int shader_type)
{
    unsigned int shader;
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success)
    {
        return shader;
    }

    const int log_len = 512;
    char log[log_len];
    glGetShaderInfoLog(shader, log_len, NULL, log);
    std::string header = "--< ";
    if (shader_type == GL_VERTEX_SHADER)
    {
        header += "vertex";
    }
    else if (shader_type == GL_FRAGMENT_SHADER)
    {
        header += "fragment";
    }
    header += " shader compilation failed >--";
    std::cout << header << std::endl << log;
    return shader;
}

void Core()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "ggj", NULL, NULL);
    LogAbortIf(!window, "glfw window creation failed");

    glfwMakeContextCurrent(window);

    bool glad_loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LogAbortIf(!glad_loaded, "glad initialization failed");

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, resize_callback);

    const char* vertex_shader_source =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\n";

    const char* fragment_shader_source =
        "#version 330 core\n"
        "out vec4 frag_color;\n"
        "void main()\n"
        "{\n"
        "  frag_color = vec4(1.0f, 0.2f, 0.3f, 1.0f);\n"
        "}";

    unsigned int vertex_shader = compile_shader(
        vertex_shader_source,
        GL_VERTEX_SHADER);
    unsigned int fragment_shader = compile_shader(
        fragment_shader_source,
        GL_FRAGMENT_SHADER);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    int success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        const int log_len = 512;
        char log[log_len];
        glGetProgramInfoLog(shader_program, log_len, NULL, log);
        std::cout << "--< shader linking failed >--" << std::endl << log;
        Error::Abort();
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glUseProgram(shader_program);

    float vertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bool active = true;
    while (active)
    {
        process_input(window);

        glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
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