target_sources(${varkorTarget} PRIVATE
  local/glad/glad.c
  local/imgui/imgui.cpp
  local/imgui/imgui_demo.cpp
  local/imgui/imgui_draw.cpp
  local/imgui/imgui_tables.cpp
  local/imgui/imgui_widgets.cpp
  local/imgui/imgui_impl_glfw.cpp
  local/imgui/imgui_impl_opengl3.cpp
  sub/StackWalker/Main/StackWalker/StackWalker.cpp)
target_include_directories(${varkorTarget} PUBLIC
  "local"
  "sub/stb"
  "sub/StackWalker/Main/StackWalker")
