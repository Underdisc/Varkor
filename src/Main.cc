#include <cmath>
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
#include "comp/DirectionalLight.h"
#include "comp/PointLight.h"
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
  Texture diffuseTexture("container_diffuse.png");
  Texture specularTexture("container_specular.png");

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
  lightTransform.SetUniformScale(0.2f);
  Comp::PointLight lightComp;

  const int objectCount = 7;
  int currentFocus = 0;
  Comp::Transform objectTransforms[objectCount];
  for (int i = 0; i < objectCount; ++i)
  {
    float radians = ((float)i * PI2f) / objectCount;
    float x = 3.0f * std::cosf(radians);
    float y = 3.0f * std::sinf(radians);
    objectTransforms[i].SetTranslation({x, y, 0.0f});
  }
  float specularExponent = 32.0f;

  Camera camera;

  while (Viewport::Active())
  {
    Framer::Start();
    Input::Update();
    Editor::Start();

    camera.Update(Temporal::DeltaTime());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Rotate the selected object using the mouse motion.
    Comp::Transform& selectedTransform = objectTransforms[currentFocus];
    Quat objectQuat = selectedTransform.GetRotation();
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
        selectedTransform.SetRotation(objectQuat);
      }
    }

    // Reset the rotation of the selected object when R is pressed.
    if (Input::KeyPressed(Input::Key::R))
    {
      Quat objectQuat = selectedTransform.GetRotation();
      objectQuat *= objectQuat.Conjugate();
      selectedTransform.SetRotation(objectQuat);
    }

    // Dispaly all adjustable values in an editor window.
    ImGui::Begin("Editor");
    ImGui::Text("Light Values");
    lightComp.EditorHook();
    lightTransform.SetTranslation(lightComp.mPosition);
    ImGui::Separator();

    ImGui::Text("Object Values");
    for (int i = 0; i < objectCount; ++i)
    {
      if (ImGui::TreeNode((void*)(intptr_t)i, "%d", i))
      {
        objectTransforms[i].EditorHook();
        if (ImGui::Button("Focus"))
        {
          currentFocus = i;
        }
        ImGui::TreePop();
      }
    }
    ImGui::Separator();
    ImGui::DragFloat("Specular Exponent", &specularExponent, 1.0f);
    ImGui::End();

    // Set all of the light uniforms and render the light.
    light.SetMat4("model", lightTransform.GetMatrix().CData());
    light.SetMat4("view", camera.WorldToCamera().CData());
    light.SetMat4("proj", Viewport::Perspective().CData());
    light.SetVec3("lightColor", lightComp.mSpecular.CData());

    glBindVertexArray(lightVao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

    // Set all of the phong uniforms and necessary texture units.
    phong.SetMat4("view", camera.WorldToCamera().CData());
    phong.SetMat4("proj", Viewport::Perspective().CData());
    phong.SetVec3("viewPos", camera.Position().CData());
    phong.SetVec3("light.position", lightComp.mPosition.CData());
    phong.SetVec3("light.ambientColor", lightComp.mAmbient.CData());
    phong.SetVec3("light.diffuseColor", lightComp.mDiffuse.CData());
    phong.SetVec3("light.specularColor", lightComp.mSpecular.CData());
    phong.SetFloat("light.constant", lightComp.mConstant);
    phong.SetFloat("light.linear", lightComp.mLinear);
    phong.SetFloat("light.quadratic", lightComp.mQuadratic);
    phong.SetSampler("material.diffuseMap", 0);
    phong.SetSampler("material.specularMap", 1);
    phong.SetFloat("material.specularExponent", specularExponent);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture.Id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularTexture.Id());

    // Render all of the cubes.
    for (int i = 0; i < objectCount; ++i)
    {
      phong.SetMat4("model", objectTransforms[i].GetMatrix().CData());
      glBindVertexArray(objectVao);
      glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
    }

    Debug::Draw::CartesianAxes();
    Debug::Draw::Render(camera.WorldToCamera(), Viewport::Perspective());

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