// glad.h must be included before glfw3.h. Including glad.h is required before
// any includes or code that contain Opengl calls.
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "camera.h"
#include "debug/draw.h"
#include "error.h"
#include "input.h"
#include "math/constants.h"
#include "math/matrix.hh"
#include "math/vector.hh"
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
  GLFWwindow* window = glfwCreateWindow(width, height, "Varkor", NULL, NULL);
  LogAbortIf(!window, "glfw window creation failed");

  glfwMakeContextCurrent(window);

  bool glad_loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  LogAbortIf(!glad_loaded, "glad initialization failed");

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, resize_callback);

  Input::Init(window);
  Debug::Draw::Init();

  // shader setup
  Shader solid("shader/solid.vs", "shader/solid.fs");

  // clang-format off
    // buffer setup
    float vertices[] = {
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f
    };
    unsigned int indicies[] = {
        0, 1, 2,
        1, 3, 2,
        4, 5, 0,
        5, 1, 0,
        4, 7, 5,
        4, 6, 7,
        6, 3, 7,
        6, 2, 3,
        1, 5, 3,
        3, 5, 7,
        0, 6, 4,
        0, 2, 6
    };
  // clang-format on

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
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  bool active = true;
  Camera camera;
  while (active)
  {
    Input::Update();
    // todo: replace this argument with a real dt.
    float dt = 1.0f / 60.0f;
    camera.Update(dt);
    process_input(window);

    active = !glfwWindowShouldClose(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // matrix transformation setup
    float rotation = PIf * (float)glfwGetTime();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, -rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    float dist = -3.0f - (float)sin(glfwGetTime());
    Mat4 view = camera.WorldToCamera();

    float fov = PIf / 4.0f;
    glm::mat4 proj = glm::mat4(1.0f);
    proj = glm::perspective(fov, (float)width / (float)height, 0.1f, 100.0f);

    // I will be replacing the model matrix using my own matrix implementation.
    solid.SetMat4("model", glm::value_ptr(model));
    solid.SetMat4("view", view.Data(), true);
    solid.SetMat4("proj", glm::value_ptr(proj));

    solid.Use();
    glBindVertexArray(vao);
    glDrawElements(
      GL_TRIANGLES,
      sizeof(indicies) / sizeof(unsigned int),
      GL_UNSIGNED_INT,
      0);
    glBindVertexArray(0);

    // Drawing the x, y, and z axis using debug drawing.
    Vec3 x = {1.0f, 0.0f, 0.0f};
    Vec3 y = {0.0f, 1.0f, 0.0f};
    Vec3 z = {0.0f, 0.0f, 1.0f};
    Vec3 o = {0.0f, 0.0f, 0.0f};
    Debug::Draw::Line(o, x, x);
    Debug::Draw::Line(o, y, y);
    Debug::Draw::Line(o, z, z);
    Mat4 tempProj;
    float* oProj = glm::value_ptr(proj);
    float* tProj = tempProj.Data();
    for (int i = 0; i < 16; ++i)
    {
      tProj[i] = oProj[i];
    }
    Debug::Draw::Render(view, tempProj);

    glfwSwapBuffers(window);
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