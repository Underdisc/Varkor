#include <cmath>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Camera.h"
#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
#include "comp/DirectionalLight.h"
#include "comp/PointLight.h"
#include "comp/SpotLight.h"
#include "comp/Transform.h"
#include "core/World.h"
#include "debug/Draw.h"
#include "editor/Primary.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "math/Constants.h"
#include "math/Matrix4.h"
#include "math/Quaternion.h"
#include "math/Vector.h"

void Engine()
{
  Input::Init();
  Core::World::Init();
  Editor::Init();
  Debug::Draw::Init();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);

  Camera camera;

  // shader setup
  Gfx::Shader phongShader("shader/phong.vs", "shader/phong.fs");
  Gfx::Shader colorShader("shader/light.vs", "shader/PureColor.fs");

  // light setup
  float specularExponent = 32.0f;
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
  Gfx::Model lightModel("res/sphere.obj");

  // object setup
  const int objectCount = 7;
  int currentFocus = 0;
  Comp::Transform objectTransforms[objectCount];
  for (int i = 0; i < objectCount; ++i)
  {
    float radians = ((float)i * Math::nTao) / objectCount;
    float x = 3.0f * std::cosf(radians);
    float y = 3.0f * std::sinf(radians);
    objectTransforms[i].SetTranslation({x, y, 0.0f});
    objectTransforms[i].SetUniformScale(0.5f);
  }
  Gfx::Texture diffuseTexture("container_diffuse.png");
  Gfx::Texture specularTexture("container_specular.png");
  Gfx::Model objectModel("res/cube.obj");

  // backpack setup
  Comp::Transform backpackTransform;
  backpackTransform.SetTranslation({0.0f, 0.0f, -5.0f});
  Gfx::Model backpackModel("res/backpack/backpack.obj");

  while (Viewport::Active())
  {
    Framer::Start();
    Input::Update();
    Editor::Start();

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    camera.Update(Temporal::DeltaTime());

    // Rotate the selected object and backpack using the mouse motion.
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
    ImGui::Begin("TempEditor");
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
    colorShader.SetMat4("uView", camera.WorldToCamera().CData());
    colorShader.SetMat4("uProj", Viewport::Perspective().CData());
    for (int i = 0; i < pointLightCount; ++i)
    {
      colorShader.SetMat4("uModel", lightTransforms[i].GetMatrix().CData());
      colorShader.SetVec3("uColor", pointLights[i].mSpecular.CData());
      lightModel.Draw(colorShader);
    }

    // Render the spotLight
    colorShader.SetMat4("uModel", lightTransforms[4].GetMatrix().CData());
    colorShader.SetVec3("uColor", spotLight.mSpecular.CData());
    lightModel.Draw(colorShader);

    // Show the direction of the directional light.
    Vec3 white = {1.0f, 1.0f, 1.0f};
    Vec3 origin = {0.0f, 0.0f, 0.0f};
    Debug::Draw::Line(origin, dirLight.mDirection, white);

    // Set all of the phong uniforms.
    phongShader.SetMat4("uView", camera.WorldToCamera().CData());
    phongShader.SetMat4("uProj", Viewport::Perspective().CData());
    phongShader.SetVec3("viewPos", camera.Position().CData());
    phongShader.SetFloat("material.specularExponent", specularExponent);
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
      phongShader.SetVec3(ssPos.str().c_str(), light.mPosition.CData());
      phongShader.SetVec3(ssAmbient.str().c_str(), light.mAmbient.CData());
      phongShader.SetVec3(ssDiffuse.str().c_str(), light.mDiffuse.CData());
      phongShader.SetVec3(ssSpecular.str().c_str(), light.mSpecular.CData());
      phongShader.SetFloat(ssConstant.str().c_str(), light.mConstant);
      phongShader.SetFloat(ssLinear.str().c_str(), light.mLinear);
      phongShader.SetFloat(ssQuadratic.str().c_str(), light.mQuadratic);
    }
    // Set the uniforms for the spot light.
    phongShader.SetVec3("spotLight.position", spotLight.mPosition.CData());
    phongShader.SetVec3("spotLight.direction", spotLight.mDirection.CData());
    phongShader.SetVec3("spotLight.ambient", spotLight.mAmbient.CData());
    phongShader.SetVec3("spotLight.diffuse", spotLight.mDiffuse.CData());
    phongShader.SetVec3("spotLight.specular", spotLight.mSpecular.CData());
    phongShader.SetFloat("spotLight.constant", spotLight.mConstant);
    phongShader.SetFloat("spotLight.linear", spotLight.mLinear);
    phongShader.SetFloat("spotLight.quadratic", spotLight.mQuadratic);
    phongShader.SetFloat("spotLight.innerCutoff", spotLight.mInnerCutoff);
    phongShader.SetFloat("spotLight.outerCutoff", spotLight.mOuterCutoff);
    // Set the uniforms for the directional light.
    phongShader.SetVec3("dirLight.direction", dirLight.mDirection.CData());
    phongShader.SetVec3("dirLight.ambient", dirLight.mAmbient.CData());
    phongShader.SetVec3("dirLight.diffuse", dirLight.mDiffuse.CData());
    phongShader.SetVec3("dirLight.specular", dirLight.mSpecular.CData());

    // Render the backpack.
    phongShader.SetMat4("uModel", backpackTransform.GetMatrix().CData());
    backpackModel.Draw(phongShader);

    // Set up the correct textures for the objects.
    phongShader.SetSampler("material.diffuseMap", 0);
    phongShader.SetSampler("material.specularMap", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture.Id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularTexture.Id());

    // Render the objects and write to the stencil buffer.
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0x10, 0xff);
    glStencilMask(0xff);
    for (int i = 0; i < objectCount; ++i)
    {
      phongShader.SetMat4("uModel", objectTransforms[i].GetMatrix().CData());
      objectModel.Draw(phongShader);
    }

    // Render the outlines of the objects.
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_NOTEQUAL, 0x10, 0xff);
    glStencilMask(0x00);
    for (int i = 0; i < objectCount; ++i)
    {
      Mat4 scaleBump;
      Math::Scale(&scaleBump, 1.1f);
      const Mat4& currentTransform = objectTransforms[i].GetMatrix();
      Mat4 newTransform = currentTransform * scaleBump;
      Vec3 red = {1.0f, 0.0f, 0.0f};
      colorShader.SetMat4("uModel", newTransform.CData());
      colorShader.SetVec3("uColor", red.CData());
      objectModel.Draw(colorShader);
    }
    glEnable(GL_DEPTH_TEST);

    // Clear the stencil buffer.
    glStencilMask(0xff);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);

    Core::World::Render(camera);

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
  Framer::Init();
  Engine();
  Framer::Purge();
  Viewport::Purge();
  Error::Purge();
  return 0;
}