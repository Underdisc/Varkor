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
#include "comp/Transform.h"
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
    // positions          normals              texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };
  // clang-format on

  // Texture setup
  Texture texture("container_diffuse.png");

  // Vertex buffer setup
  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Light vertex array setup
  unsigned int lightVao;
  glGenVertexArrays(1, &lightVao);
  glBindVertexArray(lightVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // Object vertex array setup
  unsigned int objectVao;
  glGenVertexArrays(1, &objectVao);
  glBindVertexArray(objectVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glBindVertexArray(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  Comp::Transform lightTransform;
  Comp::Transform objectTransform;
  lightTransform.SetTranslation({2.0f, 2.0f, 2.0f});
  lightTransform.SetUniformScale(0.2f);
  objectTransform.SetTranslation({0.0f, 0.0f, 0.0f});

  Vec3 lightAmbient = {0.2f, 0.2f, 0.2f};
  Vec3 lightDiffuse = {0.5f, 0.5f, 0.5f};
  Vec3 lightSpecular = {1.0f, 1.0f, 1.0f};

  Vec3 materialSpecular = {0.5f, 0.5f, 0.5f};
  float specularExponent = 32.0f;

  Camera camera;

  while (Viewport::Active())
  {
    Framer::Start();
    Editor::Start();

    Input::Update();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    camera.Update(Temporal::DeltaTime());
    const Mat4& view = camera.WorldToCamera();

    // Rotate the object using the mouse motion.
    Quat objectQuat = objectTransform.GetRotation();
    if (Input::MouseDown(Input::Mouse::Left))
    {
      Vec2 mouse = Input::MouseMotion();
      Vec3 rotAxis = (Vec3)mouse;
      float temp = rotAxis[0];
      rotAxis[0] = rotAxis[1];
      rotAxis[1] = temp;
      float mag = Math::Magnitude(rotAxis);
      if (mag != 0.0f)
      {
        const float magScaling = 0.001f;
        mag *= magScaling;
        rotAxis = Math::Normalize(rotAxis);

        Quat rot(mag, rotAxis);
        objectQuat = rot * objectQuat;
        objectTransform.SetRotation(objectQuat);
      }
    }

    // Reset the rotation of the object when R is pressed.
    if (Input::KeyPressed(Input::Key::R))
    {
      Quat objectQuat = objectTransform.GetRotation();
      objectQuat *= objectQuat.Conjugate();
      objectTransform.SetRotation(objectQuat);
    }

    // Draw the rotation axis of the object's quaternion.
    Vec3 axis = objectQuat.Axis();
    Vec3 purple = {1.0f, 0.0f, 1.0f};
    Debug::Draw::Line(-axis, axis, purple);

    // Set all of the light uniforms before rendering the light.
    light.SetMat4("model", lightTransform.GetMatrix().CData());
    light.SetMat4("view", view.CData());
    light.SetMat4("proj", Viewport::Perspective().CData());
    light.SetVec3("lightColor", lightSpecular.CData());

    glBindVertexArray(lightVao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

    // Set all of the phong uniforms and necessary texture units before
    // rendering the cube.
    phong.SetMat4("model", objectTransform.GetMatrix().CData());
    phong.SetMat4("view", view.CData());
    phong.SetMat4("proj", Viewport::Perspective().CData());
    phong.SetVec3("viewPos", camera.Position().CData());
    phong.SetVec3("light.position", lightTransform.GetTranslation().CData());
    phong.SetVec3("light.ambientColor", lightAmbient.CData());
    phong.SetVec3("light.diffuseColor", lightDiffuse.CData());
    phong.SetVec3("light.specularColor", lightSpecular.CData());
    phong.SetSampler("material.diffuseMap", 0);
    phong.SetVec3("material.specularColor", materialSpecular.CData());
    phong.SetFloat("material.specularExponent", specularExponent);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.Id());

    glBindVertexArray(objectVao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

    Debug::Draw::CartesianAxes();
    Debug::Draw::Render(view, Viewport::Perspective());

    // Dispaly all adjustable values in an editor window.
    ImGui::Begin("Editor");
    ImGui::Separator();

    Vec3 lightPos = lightTransform.GetTranslation();
    ImGui::Text("Light Values");
    ImGui::DragFloat3("Light Position", lightPos.mD, 0.01f);
    ImGui::DragFloat3("Light Ambient", lightAmbient.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat3("Light Diffuse", lightDiffuse.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat3("Light Specular", lightSpecular.mD, 0.001f, 0.0f, 1.0f);
    ImGui::Separator();
    lightTransform.SetTranslation(lightPos);

    Vec3 objectPos = objectTransform.GetTranslation();
    ImGui::Text("Object Values");
    ImGui::DragFloat3("Object Position", objectPos.mD, 0.01f);
    ImGui::DragFloat3(
      "Specular Color", materialSpecular.mD, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat("Specular Exponent", &specularExponent, 1.0f);
    objectTransform.SetTranslation(objectPos);
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