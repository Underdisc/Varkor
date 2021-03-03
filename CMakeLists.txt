cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(varkor)

# Find libraries, set properties, and set compiler flags.
find_package(OpenGL)
list(APPEND CMAKE_PREFIX_PATH "ext/lib")
if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /W3 /sdl")

  # Determine the correct libraries using the build type.
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    find_library(assimp NAMES assimp-vc142-mt REQUIRED)
    find_library(glfw NAMES glfw3_msvc2019_md REQUIRED)
  elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    find_library(assimp NAMES assimp-vc142-mtd REQUIRED)
    find_library(glfw NAMES glfw3_msvc2019_mdd REQUIRED)
  endif ()
endif ()

# Decide whether varkor is being used as a standalone executable or a library.
if ("${CMAKE_PROJECT_NAME}" STREQUAL "varkor")
  add_executable(varkor)
  add_compile_definitions(VarkorStandalone)
  set_property(TARGET varkor PROPERTY RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_SOURCE_DIR}/working")

  # The test targets are only created when varkor is used as a standalone exe.
  add_subdirectory(src/test)
  file(WRITE working/test/testNames.txt ${testNames})
else ()
  add_library(varkor)
endif ()

# Add all of the source files, include directories, and libraries needed to
# build Varkor.
add_subdirectory(src)
add_subdirectory(ext)
target_include_directories(varkor PUBLIC "src" "ext/local" "ext/sub")
target_link_libraries(varkor ${assimp} ${glfw} ${OPENGL_gl_LIBRARY})
