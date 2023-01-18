cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(varkor)

# Set the build type postfix that's appended to the output executable names.
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(buildTypePostfix "dbg")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  set(buildTypePostfix "rel")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  set(buildTypePostfix "relDbg")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
  set(buildTypePostfix "relMin")
endif()

# Create the target for the Varkor static library.
add_library(varkor STATIC)
add_subdirectory(src)
add_subdirectory(ext)

find_package(OpenGL)
target_link_libraries(varkor ${OPENGL_gl_LIBRARY})
target_include_directories(varkor PUBLIC "src")

if (MSVC)
  target_compile_options(varkor PRIVATE /WX /W3 /sdl /permissive- /std:c++17)
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    target_link_options(varkor PRIVATE /MANIFEST:NO)
  endif ()
endif ()

# Create the target for the standalone version of Varkor.
add_executable(varkorStandalone)
target_sources(varkorStandalone PRIVATE src/Main.cc)
set_target_properties(varkorStandalone PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/working"
  OUTPUT_NAME "varkorStandalone_${buildTypePostfix}")
target_link_libraries(varkorStandalone varkor)

# Create all of the test targets.
add_subdirectory(src/test)

# Create all of the targets for different Varkor projects.
function(AddProject targetName directory)
  project(${targetName})
  add_executable(${targetName})
  add_subdirectory(${directory}/src ${targetName})
  target_link_libraries(${targetName} varkor)
  target_link_options(${targetName} PRIVATE /MANIFEST:NO)
  target_compile_definitions(${targetName}
    PUBLIC PROJECT_DIRECTORY="${directory}")
  set_target_properties(${targetName} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/working"
    OUTPUT_NAME "${targetName}_${buildTypePostfix}")
endfunction()
include("projects.cmake" OPTIONAL)
