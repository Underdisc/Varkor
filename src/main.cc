// glad.h must be included before glfw3.h. Including glad.h is required before
// any includes or code that contain Opengl calls.
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
#include "time.h"
#include "texture.h"
#include "viewport.h"

void Core()
{
  Framer::SetFramerate(120);
  Input::Init();
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
  Complex complexRot = {1.0f, 0.0f};
  Complex complexRotInc = Math::ComplexPolar(1.0f, PIf / 120.0f);

  Quat cubeRot = {1.0f, 0.0, 0.0f, 0.0f};
  Vec3 axis = {1.0f, 1.0f, 1.0f};
  axis = Math::Normalize(axis);
  Quat cubeRotInc;
  cubeRotInc.AngleAxis(PIf / 120.0f, axis);

  while (Viewport::Active())
  {
    Framer::Start();
    Input::Update();

    camera.Update(Time::DeltaTime());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Crete the cube rotation matrix.
    cubeRot *= cubeRotInc;
    Mat4 model;
    Math::Rotate(&model, cubeRot);

    const Mat4& view = camera.WorldToCamera();
    solid.SetMat4("model", model.CData(), true);
    solid.SetMat4("view", view.CData(), true);
    solid.SetMat4("proj", Viewport::Perspective().CData(), true);

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

    // Drawing the complex number vector using debug drawing.
    complexRot *= complexRotInc;
    Vec3 c = {complexRot.mReal, complexRot.mImaginary, 0.0f};
    Vec3 white = {1.0f, 1.0f, 1.0f};
    Debug::Draw::Line(o, c, white);

    // Drawing the cube rotation axis.
    Vec3 purple = {1.0f, 0.0f, 1.0f};
    Debug::Draw::Line(o, axis, purple);

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