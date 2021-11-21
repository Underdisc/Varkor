#include "AssetLibrary.h"
#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Options.h"
#include "Registrar.h"
#include "Viewport.h"
#include "debug/Draw.h"
#include "editor/Editor.h"
#include "gfx/Renderer.h"
#include "world/World.h"

void VarkorMain(const char* windowName, int argc, char* argv[])
{
  // Perform all of the necessary initialization.
  Error::Init("log.err");
  Result result = Options::Init(argc, argv);
  if (!result.Success())
  {
    return;
  }
  Viewport::Init(windowName);
  Registrar::Init();
  Editor::Init();
  Framer::Init();
  AssetLibrary::Init();
  Input::Init();
  Gfx::Renderer::Init();
  Debug::Draw::Init();

  // Run the engine.
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

  // Perform all of the necessary purging.
  Framer::Purge();
  Editor::Purge();
  Viewport::Purge();
  Error::Purge();
}

#ifdef VarkorStandalone
int main(int argc, char* argv[])
{
  VarkorMain("Varkor", argc, argv);
  return 0;
}
#endif