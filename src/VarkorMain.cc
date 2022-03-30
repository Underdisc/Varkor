#include "AssetLibrary.h"
#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Options.h"
#include "Registrar.h"
#include "Result.h"
#include "Temporal.h"
#include "Viewport.h"
#include "debug/Draw.h"
#include "editor/Editor.h"
#include "gfx/Renderer.h"
#include "world/World.h"

Result VarkorInit(
  int argc, char* argv[], const char* windowName, const char* projectDirectory)
{
  Result result = Options::Init(argc, argv, projectDirectory);
  if (!result.Success())
  {
    return result;
  }
  Error::Init("log.err");
  Viewport::Init(windowName);
  Registrar::Init();
  Editor::Init();
  Framer::Init();
  AssetLibrary::Init();
  Input::Init();
  Gfx::Renderer::Init();
  Debug::Draw::Init();
  return Result();
}

void VarkorRun()
{
  while (Viewport::Active() || AssetLibrary::InitThreadOpen())
  {
    Framer::Start();

    Temporal::Update();
    Input::Update();
    Editor::StartFrame();
    World::Update();
    Gfx::Renderer::Render();
    Editor::EndFrame();
    Viewport::SwapBuffers();
    Viewport::Update();
    AssetLibrary::HandleAssetLoading();

    Framer::End();
  }
}

void VarkorPurge()
{
  World::Purge();
  Gfx::Renderer::Purge();
  AssetLibrary::Purge();
  Framer::Purge();
  Editor::Purge();
  Viewport::Purge();
  Error::Purge();
}

#ifdef VarkorStandalone
int main(int argc, char* argv[])
{
  Result result = VarkorInit(argc, argv, "Varkor Standalone", "");
  if (!result.Success())
  {
    return 0;
  }
  VarkorRun();
  VarkorPurge();
}
#endif