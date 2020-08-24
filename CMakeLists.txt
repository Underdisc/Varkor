cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(varkor VERSION 0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/working")

add_executable(varkor)
add_subdirectory(src)
target_sources(varkor PRIVATE "ext/src/glad.c")

find_package(OpenGL)

list(APPEND CMAKE_PREFIX_PATH "ext/lib")

if (MINGW)
  find_library(glfw NAMES glfw3_mingw_32 REQUIRED)

elseif (MSVC)
  set_property(TARGET varkor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
  endif ()

  if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    find_library(glfw NAMES glfw3_msvc2019_md_32 REQUIRED)
  else ()
    find_library(glfw NAMES glfw3_msvc2019_md_64 REQUIRED)
  endif ()
endif ()

target_include_directories(varkor PRIVATE "ext/inc" "ext/sub")
target_link_libraries(varkor ${glfw} ${OPENGL_gl_LIBRARY})
