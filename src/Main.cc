#include <glad/glad.h>
#include <imgui/imgui.h>
#include <iostream>
#include <string>

#include "Camera.h"
#include "Editor.h"
#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Shader.h"
#include "Temporal.h"
#include "Texture.h"
#include "Viewport.h"
#include "debug/Draw.h"
#include "math/Constants.h"
#include "math/Matrix4.h"
#include "math/Quaternion.h"
#include "math/Vector.h"

void Core()
{
  Framer::SetFramerate(120);
  Input::Init();
  Debug::Draw::Init();
  Editor::Init();

  // shader setup
  Shader phong("shader/phong.vs", "shader/phong.fs");
  Shader light("shader/light.vs", "shader/light.fs");

  // clang-format off
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
  };
  // clang-format on

  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  unsigned int lightVao;
  glGenVertexArrays(1, &lightVao);
  glBindVertexArray(lightVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  unsigned int objectVao;
  glGenVertexArrays(1, &objectVao);
  glBindVertexArray(objectVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  Mat4 objectModel;
  Math::Identity(&objectModel);

  Vec3 lightPos = {2.0f, 2.0f, 2.0f};
  Vec3 lightAmbient = {0.2f, 0.2f, 0.2f};
  Vec3 lightDiffuse = {0.5f, 0.5f, 0.5f};
  Vec3 lightSpecular = {1.0f, 1.0f, 1.0f};

  Vec3 materialAmbient = {1.0f, 0.5f, 0.31f};
  Vec3 materialDiffuse = {1.0f, 0.5f, 0.31f};
  Vec3 materialSpecular = {0.5f, 0.5f, 0.5f};
  float specularExponent = 32.0f;

  Camera camera;

  while (Viewport::Active())
  {
    Framer::Start();
    Input::Update();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Update(Temporal::DeltaTime());
    const Mat4& view = camera.WorldToCamera();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Mat4 lightScaleMat;
    Mat4 lightTransMat;
    Math::Scale(&lightScaleMat, 0.2f);
    Math::Translate(&lightTransMat, lightPos);
    Mat4 lightModel = lightTransMat * lightScaleMat;

    light.SetMat4("model", lightModel.CData());
    light.SetMat4("view", view.CData());
    light.SetMat4("proj", Viewport::Perspective().CData());
    light.SetVec3("lightColor", lightSpecular.CData());

    glBindVertexArray(lightVao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

    phong.SetMat4("model", objectModel.CData());
    phong.SetMat4("view", view.CData());
    phong.SetMat4("proj", Viewport::Perspective().CData());

    phong.SetVec3("viewPos", camera.Position().CData());
    phong.SetVec3("light.position", lightPos.CData());
    phong.SetVec3("light.ambientColor", lightAmbient.CData());
    phong.SetVec3("light.diffuseColor", lightDiffuse.CData());
    phong.SetVec3("light.specularColor", lightSpecular.CData());
    phong.SetVec3("material.ambientColor", materialAmbient.CData());
    phong.SetVec3("material.diffuseColor", materialDiffuse.CData());
    phong.SetVec3("material.specularColor", materialSpecular.CData());
    phong.SetFloat("material.specularExponent", specularExponent);

    glBindVertexArray(objectVao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

    Debug::Draw::CartesianAxes();
    Debug::Draw::Render(view, Viewport::Perspective());

    Editor::Start();
    ImGui::Begin("Editor");
    ImGui::Separator();
    ImGui::Text("Light Values");
    ImGui::DragFloat3("Light Position", lightPos.mD, 0.001f);
    ImGui::DragFloat3("Light Ambient", lightAmbient.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat3("Light Diffuse", lightDiffuse.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat3("Light Specular", lightSpecular.mD, 0.001f, 0.0f, 1.0f);
    ImGui::Separator();
    ImGui::Text("Object Values");
    ImGui::DragFloat3("Ambient Color", materialAmbient.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat3("Diffuse Color", materialDiffuse.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat3(
      "Specular Color", materialSpecular.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat("Specular Exponent", &specularExponent, 1.0f);
    ImGui::End();
    Editor::End();

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