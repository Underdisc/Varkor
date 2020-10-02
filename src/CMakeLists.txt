target_sources(varkor PRIVATE
  camera.cc
  editor.cc
  error.cc
  framer.cc
  input.cc
  main.cc
  shader.cc
  texture.cc
  time.cc
  viewport.cc)
add_subdirectory(debug)
add_subdirectory(math)
