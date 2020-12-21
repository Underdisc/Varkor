#include <cmath>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Camera.h"
#include "Editor.h"
#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
#include "comp/DirectionalLight.h"
#include "comp/PointLight.h"
#include "comp/SpotLight.h"
#include "comp/Transform.h"
#include "debug/Draw.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
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

  Comp::Transform backpackTransform;
  backpackTransform.SetTranslation({0.0f, 0.0f, -5.0f});
  Gfx::Model backpack("res/backpack/backpack.obj");

  // shader setup
  Gfx::Shader phong("shader/phong.vs", "shader/phong.fs");
  Gfx::Shader lightShader("shader/light.vs", "shader/light.fs");

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
  Gfx::Texture diffuseTexture("container_diffuse.png");
  Gfx::Texture specularTexture("container_specular.png");

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

  const int positionLightCount = 5;
  const int pointLightCount = 4;
  Comp::Transform lightTransforms[positionLightCount];
  for (int i = 0; i < positionLightCount; ++i)
  {
    lightTransforms[i].SetUniformScale(0.2f);
  }
  Comp::PointLight pointLights[pointLightCount];
  Comp::SpotLight spotLight;
  Comp::DirectionalLight dirLight;

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

    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
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

        Quat backpackRot = backpackTransform.GetRotation();
        backpackRot = rot * backpackRot;
        backpackTransform.SetRotation(backpackRot);
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
    if (ImGui::TreeNode("Lights"))
    {
      for (int i = 0; i < pointLightCount; ++i)
      {
        if (ImGui::TreeNode((void*)(intptr_t)i, "Point Light %d", i))
        {
          pointLights[i].EditorHook();
          ImGui::TreePop();
        }
        lightTransforms[i].SetTranslation(pointLights[i].mPosition);
      }
      if (ImGui::TreeNode("Spot Light"))
      {
        spotLight.EditorHook();
        lightTransforms[4].SetTranslation(spotLight.mPosition);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Directional Light"))
      {
        dirLight.EditorHook();
        ImGui::TreePop();
      }
      ImGui::TreePop();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Objects"))
    {
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
      ImGui::TreePop();
    }
    ImGui::Separator();
    ImGui::DragFloat("Specular Exponent", &specularExponent, 1.0f);
    ImGui::End();

    // Render all of the point lights.
    glBindVertexArray(lightVao);
    lightShader.SetMat4("view", camera.WorldToCamera().CData());
    lightShader.SetMat4("proj", Viewport::Perspective().CData());
    for (int i = 0; i < pointLightCount; ++i)
    {
      lightShader.SetMat4("model", lightTransforms[i].GetMatrix().CData());
      lightShader.SetVec3("lightColor", pointLights[i].mSpecular.CData());
      glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
    }

    // Render the spotLight
    lightShader.SetMat4("model", lightTransforms[4].GetMatrix().CData());
    lightShader.SetVec3("lightColor", spotLight.mSpecular.CData());
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

    // Show the direction of the directional light.
    Vec3 white = {1.0f, 1.0f, 1.0f};
    Vec3 origin = {0.0f, 0.0f, 0.0f};
    Debug::Draw::Line(origin, dirLight.mDirection, white);

    // Set all of the phong uniforms.
    phong.SetMat4("view", camera.WorldToCamera().CData());
    phong.SetMat4("proj", Viewport::Perspective().CData());
    phong.SetVec3("viewPos", camera.Position().CData());

    // Set all of the uniforms for the point lights.
    for (int i = 0; i < pointLightCount; ++i)
    {
      std::stringstream ssPos, ssAmbient, ssDiffuse, ssSpecular, ssConstant,
        ssLinear, ssQuadratic;
      ssPos << "pointLights[" << i << "].position";
      ssAmbient << "pointLights[" << i << "].ambient";
      ssDiffuse << "pointLights[" << i << "].diffuse";
      ssSpecular << "pointLights[" << i << "].specular";
      ssConstant << "pointLights[" << i << "].constant";
      ssLinear << "pointLights[" << i << "].linear";
      ssQuadratic << "pointLights[" << i << "].quadratic";

      const Comp::PointLight& light = pointLights[i];
      phong.SetVec3(ssPos.str().c_str(), light.mPosition.CData());
      phong.SetVec3(ssAmbient.str().c_str(), light.mAmbient.CData());
      phong.SetVec3(ssDiffuse.str().c_str(), light.mDiffuse.CData());
      phong.SetVec3(ssSpecular.str().c_str(), light.mSpecular.CData());
      phong.SetFloat(ssConstant.str().c_str(), light.mConstant);
      phong.SetFloat(ssLinear.str().c_str(), light.mLinear);
      phong.SetFloat(ssQuadratic.str().c_str(), light.mQuadratic);
    }

    // Set the uniforms for the spot light.
    phong.SetVec3("spotLight.position", spotLight.mPosition.CData());
    phong.SetVec3("spotLight.direction", spotLight.mDirection.CData());
    phong.SetVec3("spotLight.ambient", spotLight.mAmbient.CData());
    phong.SetVec3("spotLight.diffuse", spotLight.mDiffuse.CData());
    phong.SetVec3("spotLight.specular", spotLight.mSpecular.CData());
    phong.SetFloat("spotLight.constant", spotLight.mConstant);
    phong.SetFloat("spotLight.linear", spotLight.mLinear);
    phong.SetFloat("spotLight.quadratic", spotLight.mQuadratic);
    phong.SetFloat("spotLight.innerCutoff", spotLight.mInnerCutoff);
    phong.SetFloat("spotLight.outerCutoff", spotLight.mOuterCutoff);

    // Set the uniforms for the directional light.
    phong.SetVec3("dirLight.direction", dirLight.mDirection.CData());
    phong.SetVec3("dirLight.ambient", dirLight.mAmbient.CData());
    phong.SetVec3("dirLight.diffuse", dirLight.mDiffuse.CData());
    phong.SetVec3("dirLight.specular", dirLight.mSpecular.CData());

    // Set the material uniforms and bind the correct textures to the texture
    // units.
    phong.SetSampler("material.diffuseMap", 0);
    phong.SetSampler("material.specularMap", 1);
    phong.SetFloat("material.specularExponent", specularExponent);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture.Id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularTexture.Id());

    // Render all of the cubes.
    glBindVertexArray(objectVao);
    for (int i = 0; i < objectCount; ++i)
    {
      phong.SetMat4("model", objectTransforms[i].GetMatrix().CData());
      glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
    }

    // Render the backpack.
    phong.SetMat4("model", backpackTransform.GetMatrix().CData());
    backpack.Draw(phong);

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