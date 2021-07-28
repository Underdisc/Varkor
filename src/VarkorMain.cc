#include "AssetLibrary.h"
#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Viewport.h"
#include "comp/Registrar.h"
#include "debug/Draw.h"
#include "editor/Primary.h"
#include "gfx/Renderer.h"
#include "world/World.h"

void VarkorInit(const char* windowName)
{
  Error::Init("log.err");
  Viewport::Init(windowName);
  Editor::Init();
  Framer::Init();
  AssetLibrary::LoadAssets();
  Comp::Init();
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
    Editor::Start();
    Gfx::Renderer::Clear();

    Editor::Run();
    World::Update();
    if (!Editor::EditorMode())
    {
      Gfx::Renderer::RenderWorld();
    }

    Gfx::Renderer::RenderQueuedFullscreenFramebuffers();
    Editor::End();
    Viewport::SwapBuffers();
    Viewport::Update();
    Framer::End();
  }
}

void VarkorPurge()
{
  AssetLibrary::SaveAssets();
  Framer::Purge();
  Editor::Purge();
  Viewport::Purge();
  Error::Purge();
}

#ifdef VarkorStandalone
int main(void)
{
  VarkorInit("Varkor");
  VarkorEngine();
  VarkorPurge();
  return 0;
}
#endif