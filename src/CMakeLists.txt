target_sources(varkor PRIVATE
  AssetLibrary.cc
  Camera.cc
  Error.cc
  Framer.cc
  Input.cc
  Profiler.cc
  Temporal.cc
  VarkorMain.cc
  Viewport.cc)
add_subdirectory(comp)
add_subdirectory(debug)
add_subdirectory(editor)
add_subdirectory(gfx)
add_subdirectory(math)
add_subdirectory(world)
