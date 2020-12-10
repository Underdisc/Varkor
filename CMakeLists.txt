cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(varkor)

# Find libraries, set properties, and set compiler flags.
find_package(OpenGL)
list(APPEND CMAKE_PREFIX_PATH "ext/lib")
if (MINGW)
  find_library(glfw NAMES glfw3_mingw_32 REQUIRED)
elseif (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /W3 /sdl")

  # Determine the correct libraries using the build type and the pointer size.
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
      find_library(glfw NAMES assimp_msvc2019_md_32 REQUIRED)
      find_library(glfw NAMES glfw3_msvc2019_md_32 REQUIRED)
    else ()
      find_library(glfw NAMES assimp_msvc2019_md_64 REQUIRED)
      find_library(glfw NAMES glfw3_msvc2019_md_64 REQUIRED)
    endif ()
  elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
      find_library(glfw NAMES assimp_msvc2019_mdd_32 REQUIRED)
      find_library(glfw NAMES glfw3_msvc2019_mdd_32 REQUIRED)
    else ()
      find_library(glfw NAMES assimp_msvc2019_mdd_64 REQUIRED)
      find_library(glfw NAMES glfw3_msvc2019_mdd_64 REQUIRED)
    endif ()
  endif ()
endif ()

# Create the target to build Varkor.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/working")
add_executable(varkor)
add_subdirectory(src)
add_subdirectory(ext)
target_include_directories(varkor PRIVATE "src" "ext/local" "ext/sub")
target_link_libraries(varkor ${glfw} ${OPENGL_gl_LIBRARY})
set_property(TARGET varkor PROPERTY RUNTIME_OUTPUT_DIRECTORY
  "${CMAKE_SOURCE_DIR}/working")

# Create all the test targets.
add_subdirectory(src/test)
