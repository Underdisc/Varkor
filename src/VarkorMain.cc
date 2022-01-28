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

Result VarkorInit(const char* windowName, int argc, char* argv[])
{
  Result result = Options::Init(argc, argv);
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
    Gfx::Renderer::Clear();
    Editor::Run();
    World::Update();
    if (!Editor::nEditorMode)
    {
      Gfx::Renderer::RenderWorld();
    }
    Gfx::Renderer::RenderQueuedFullscreenFramebuffers();
    Editor::Render();
    Viewport::SwapBuffers();
    Viewport::Update();
    AssetLibrary::HandleAssetLoading();

    Framer::End();
  }
}

void VarkorPurge()
{
  AssetLibrary::Purge();
  Framer::Purge();
  Editor::Purge();
  Viewport::Purge();
  Error::Purge();
}

#ifdef VarkorStandalone
int main(int argc, char* argv[])
{
  Result result = VarkorInit("Varkor Standalone", argc, argv);
  if (!result.Success())
  {
    return 0;
  }
  VarkorRun();
  VarkorPurge();
}
#endif