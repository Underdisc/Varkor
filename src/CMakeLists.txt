target_sources(varkor PRIVATE
  Error.cc
  Framer.cc
  Input.cc
  Log.cc
  Options.cc
  Result.cc
  Temporal.cc
  VarkorMain.cc
  Viewport.cc)
add_subdirectory(comp)
add_subdirectory(ds)
add_subdirectory(debug)
add_subdirectory(editor)
add_subdirectory(gfx)
add_subdirectory(math)
add_subdirectory(rsl)
add_subdirectory(target)
add_subdirectory(test)
add_subdirectory(vlk)
add_subdirectory(world)

if (WIN32)
  set_source_files_properties(Log.cc TARGET_DIRECTORY varkor
    PROPERTIES COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
endif()