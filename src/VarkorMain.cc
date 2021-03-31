#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Viewport.h"
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

    // We must clear the render buffer before showing the editor because the
    // editor may perform draw calls.
    Gfx::Renderer::Clear();
    Editor::Show();

    World::Update();
    if (!Editor::EditorMode())
    {
      Gfx::Renderer::RenderWorld();
    }

    Editor::End();
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
  VarkorInit("Varkor");
  VarkorEngine();
  VarkorPurge();
  return 0;
}
#endif