target_sources(varkor PRIVATE
  Camera.cc
  Editor.cc
  Error.cc
  Framer.cc
  Input.cc
  Main.cc
  Shader.cc
  Texture.cc
  Temporal.cc
  Viewport.cc)
add_subdirectory(comp)
add_subdirectory(debug)
add_subdirectory(math)
