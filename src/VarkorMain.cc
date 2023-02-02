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
#include "ext/Tracy.h"
#include "gfx/GlError.h"
#include "gfx/Renderer.h"
#include "gfx/UniformVector.h"
#include "rsl/Library.h"
#include "world/World.h"

Result VarkorInit(
  int argc, char* argv[], const char* windowName, const char* projectDirectory)
{
  ProfileThread("Main");

  Result result = Options::Init(argc, argv, projectDirectory);
  if (!result.Success()) {
    return result;
  }
  Error::Init("log.err");
  Viewport::Init(windowName);
  Gfx::GlError::Init();
  Gfx::UniformVector::Init();
  Rsl::Init();
  Gfx::Renderer::Init();
  Registrar::Init();
  Editor::Init();
  Framer::Init();
  Input::Init();
  Debug::Draw::Init();

  return Result();
}

void VarkorRun()
{
  while (Viewport::Active() || Rsl::InitThreadOpen()) {
    Framer::Start();

    Temporal::Update();
    Input::Update();
    Editor::StartFrame();
    World::Update();
    Gfx::Renderer::Render();
    Editor::EndFrame();
    Viewport::SwapBuffers();
    Viewport::Update();
    Rsl::HandleInitialization();

    Framer::End();

    FrameMark;
  }
}

void VarkorPurge()
{
  World::Purge();
  Gfx::Renderer::Purge();
  Rsl::Purge();
  Framer::Purge();
  Editor::Purge();
  Viewport::Purge();
  Error::Purge();
}