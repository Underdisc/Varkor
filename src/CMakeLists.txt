target_sources(${varkorTarget} PRIVATE
  AssetLibrary.cc
  Error.cc
  Framer.cc
  Input.cc
  Options.cc
  Profiler.cc
  Registrar.cc
  Temporal.cc
  VarkorMain.cc
  Viewport.cc)
add_subdirectory(comp)
add_subdirectory(debug)
add_subdirectory(editor)
add_subdirectory(gfx)
add_subdirectory(math)
add_subdirectory(vlk)
add_subdirectory(world)
