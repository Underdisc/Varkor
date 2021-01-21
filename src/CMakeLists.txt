target_sources(varkor PRIVATE
  Camera.cc
  Editor.cc
  Error.cc
  Framer.cc
  Input.cc
  Main.cc
  Profiler.cc
  Temporal.cc
  Viewport.cc)
add_subdirectory(comp)
add_subdirectory(core)
add_subdirectory(debug)
add_subdirectory(gfx)
add_subdirectory(math)
