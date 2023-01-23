target_include_directories(varkor PUBLIC "local")

# glad
target_sources(varkor PRIVATE local/glad/glad.c)

# getopt
target_sources(varkor PRIVATE local/mingw/getopt.c)
target_include_directories(varkor PUBLIC "local/mingw")

# StackWalker
add_library(stackWalker)
target_sources(stackWalker PRIVATE
  sub/StackWalker/Main/StackWalker/StackWalker.cpp)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(stackWalker PRIVATE -Wno-switch)
endif()
target_link_libraries(varkor stackWalker)
target_include_directories(varkor PUBLIC "sub/StackWalker/Main/StackWalker")

# stb
target_include_directories(varkor PUBLIC "sub/stb")

# imgui
target_sources(varkor PRIVATE
  local/imgui/imgui.cpp
  local/imgui/imgui_demo.cpp
  local/imgui/imgui_draw.cpp
  local/imgui/imgui_tables.cpp
  local/imgui/imgui_widgets.cpp
  local/imgui/imgui_impl_glfw.cpp
  local/imgui/imgui_impl_opengl3.cpp)

# assimp
if (MSVC)
  set(CMAKE_RC_FLAGS "/nologo")
endif ()
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "")
set(ASSIMP_INSTALL OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
add_subdirectory(sub/assimp)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_definitions(zlibstatic PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_options(zlibstatic PRIVATE -Wno-deprecated-declarations)
endif()
target_link_libraries(varkor assimp)
target_include_directories(varkor PUBLIC "sub/assimp/include")

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_INSTALL OFF CACHE BOOL "")
add_subdirectory(sub/glfw)
target_link_libraries(varkor glfw)
target_include_directories(varkor PUBLIC "sub/glfw/include")
