#include "Error.h"
#include "Framer.h"
#include "Input.h"
#include "Log.h"
#include "Options.h"
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

Result VarkorInit(int argc, char* argv[], Options::Config&& config) {
  ProfileThread("Main");

  Result result = Options::Init(argc, argv, std::move(config));
  if (!result.Success()) {
    return result;
  }
  Log::Init("log.txt");
  Error::Init();
  Viewport::Init(Options::nConfig.mWindowName.c_str());
  Gfx::GlError::Init();
  Gfx::UniformVector::Init();
  Rsl::Init();
  Gfx::Renderer::Init();
  World::Init();
  Editor::Init();
  Framer::Init();
  Input::Init();
  Debug::Draw::Init();

  return Result();
}

void VarkorRun() {
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

void VarkorPurge() {
  Editor::Purge();
  World::Purge();
  Gfx::Renderer::Purge();
  Rsl::Purge();
  Framer::Purge();
  Viewport::Purge();
  Log::Purge();
}
