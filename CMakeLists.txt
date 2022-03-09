cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(varkor)

# Find libraries, set properties, and set compiler flags.
find_package(OpenGL)
list(APPEND CMAKE_PREFIX_PATH "ext/lib")
if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /W3 /sdl /std:c++17")

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

function(AddVarkorTarget varkorTarget)
  add_subdirectory(src ${varkorTarget}Src)
  add_subdirectory(ext ${varkorTarget}Ext)
  target_include_directories(${varkorTarget} PUBLIC "src")
  target_link_libraries(${varkorTarget} ${assimp} ${glfw} ${OPENGL_gl_LIBRARY})
endfunction()

# Create the target for the standalone version of Varkor.
add_executable(varkor)
target_compile_definitions(varkor PRIVATE VarkorStandalone)
set_property(TARGET varkor PROPERTY RUNTIME_OUTPUT_DIRECTORY
  "${CMAKE_SOURCE_DIR}/working")
AddVarkorTarget(varkor)

# Create the target for the library version of Varkor.
add_library(varkorLibrary)
AddVarkorTarget(varkorLibrary)

# Create all of the test targets.
add_subdirectory(src/test)

function(AddProject targetName directory)
  project(${targetName})
  add_executable(${targetName})
  add_subdirectory("${directory}/src" "${targetName}Src")
  target_link_libraries(${targetName} varkorLibrary)
  target_compile_definitions(${targetName}
    PUBLIC PROJECT_DIRECTORY="${directory}")
  set_property(TARGET ${targetName} PROPERTY RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_SOURCE_DIR}/working")
endfunction()

# Create all of the targets for different Varkor projects.
include("projects.cmake" OPTIONAL)
