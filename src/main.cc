#include <glad/glad.h>

#include <iostream>
#include <string>

#include "camera.h"
#include "debug/draw.h"
#include "error.h"
#include "framer.h"
#include "input.h"
#include "math/complex.hh"
#include "math/constants.h"
#include "math/matrix_4.h"
#include "math/quaternion.h"
#include "math/vector.hh"
#include "shader.h"
#include "texture.h"
#include "time.h"
#include "viewport.h"

void Core()
{
  Framer::SetFramerate(120);
  Input::Init();
  Debug::Draw::Init();

  // shader setup
  Shader phong("shader/phong.vs", "shader/phong.fs");
  Shader light("shader/light.vs", "shader/light.fs");

  // clang-format off
  // buffer setup
  float vertices[] = {
      -0.5f,  0.5f,  0.5f,
       0.5f,  0.5f,  0.5f,
      -0.5f, -0.5f,  0.5f,
       0.5f, -0.5f,  0.5f,
      -0.5f,  0.5f, -0.5f,
       0.5f,  0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
       0.5f, -0.5f, -0.5f
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  Vec3 lightColor = {0.0f, 1.0f, 1.0f};
  Vec3 lightPos = {2.0f, 2.0f, 2.0f};
  Mat4 lightScaleMat;
  Mat4 lightTransMat;
  Math::Scale(&lightScaleMat, 0.2f);
  Math::Translate(&lightTransMat, lightPos);
  Mat4 lightModel = lightTransMat * lightScaleMat;

  Vec3 objectColor = {1.0f, 1.0f, 0.0f};
  Mat4 objectModel;
  Math::Identity(&objectModel);

  Camera camera;

  while (Viewport::Active())
  {
    Framer::Start();
    Input::Update();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Update(Time::DeltaTime());
    const Mat4& view = camera.WorldToCamera();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(vao);

    light.SetMat4("model", lightModel.CData(), true);
    light.SetMat4("view", view.CData(), true);
    light.SetMat4("proj", Viewport::Perspective().CData(), true);
    light.SetVec3("lightColor", lightColor.CData());

    glDrawElements(
      GL_TRIANGLES,
      sizeof(indicies) / sizeof(unsigned int),
      GL_UNSIGNED_INT,
      0);

    phong.SetMat4("model", objectModel.CData(), true);
    phong.SetMat4("view", view.CData(), true);
    phong.SetMat4("proj", Viewport::Perspective().CData(), true);
    phong.SetVec3("lightColor", lightColor.CData());
    phong.SetVec3("objectColor", objectColor.CData());

    glDrawElements(
      GL_TRIANGLES,
      sizeof(indicies) / sizeof(unsigned int),
      GL_UNSIGNED_INT,
      0);
    glBindVertexArray(0);

    Debug::Draw::CartesianAxes();
    Debug::Draw::Render(view, Viewport::Perspective());
    Viewport::SwapBuffers();
    Viewport::Update();

    Framer::End();
  }
}

int main(void)
{
  Error::Init("log.err");
  Viewport::Init();
  Core();
  Viewport::Purge();
  Error::Purge();
  return 0;
}