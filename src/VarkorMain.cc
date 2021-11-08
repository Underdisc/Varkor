#include "AssetLibrary.h"
#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Registrar.h"
#include "Viewport.h"
#include "debug/Draw.h"
#include "debug/MemLeak.h"
#include "editor/Editor.h"
#include "gfx/Renderer.h"
#include "world/World.h"

void VarkorInit(const char* windowName)
{
  Error::Init("log.err");
  Viewport::Init(windowName);
  Editor::Init();
  Framer::Init();
  AssetLibrary::Init();
  Registrar::Init();
}

void VarkorEngine()
{
  Input::Init();
  Gfx::Renderer::Init();
  Debug::Draw::Init();

  while (Viewport::Active())
  {
    Framer::Start();
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
    Framer::End();
  }
}

void VarkorPurge()
{
  Framer::Purge();
  Editor::Purge();
  Viewport::Purge();
  Error::Purge();
}

#ifdef VarkorStandalone
int main(void)
{
  InitMemLeakOutput();
  VarkorInit("Varkor");
  VarkorEngine();
  VarkorPurge();
  return 0;
}
#endif