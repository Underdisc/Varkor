target_include_directories(varkor PUBLIC "local")
target_include_directories(varkor PUBLIC "sub")

# dirent
if (WIN32)
  target_include_directories(varkor PRIVATE local/dirent)
endif()

# glad
target_sources(varkor PRIVATE local/glad/src/glad.c)
target_include_directories(varkor PUBLIC local/glad/include)

# getopt
if (WIN32)
  target_sources(varkor PRIVATE local/mingw/getopt.c)
  target_include_directories(varkor PUBLIC "local/mingw")
endif()

# backward-cpp
add_subdirectory(sub/backward-cpp)

# stb
target_include_directories(varkor PUBLIC "sub/stb")

# imgui
target_sources(varkor PRIVATE
  sub/imgui/imgui.cpp
  sub/imgui/imgui_demo.cpp
  sub/imgui/imgui_draw.cpp
  sub/imgui/imgui_tables.cpp
  sub/imgui/imgui_widgets.cpp
  sub/imgui/backends/imgui_impl_glfw.cpp
  sub/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(varkor PUBLIC "sub/imgui")

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

# tracy
target_include_directories(varkor PUBLIC "sub/tracy/public/tracy")
if("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  set(TRACY_ENABLE ON)
  set(TRACY_ON_DEMAND OFF)
  set(TRACY_NO_EXIT ON)
  add_subdirectory(sub/tracy)
endif()

function(add_externals target)
  add_backward(${target})
  if("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    target_link_libraries(${target} Tracy::TracyClient)
  endif()
endfunction()