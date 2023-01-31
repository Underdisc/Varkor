cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(varkor)

# Create the target for the Varkor static library.
add_library(varkor STATIC)
add_subdirectory(src)
add_subdirectory(ext)

find_package(OpenGL)
target_link_libraries(varkor ${OPENGL_gl_LIBRARY})
target_include_directories(varkor PUBLIC "src")

# Set compiler options.
target_compile_features(varkor PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(varkor PRIVATE /WX /W3 /sdl /permissive-)
  if("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    target_link_options(varkor PRIVATE /MANIFEST:NO)
  endif()
endif()

# Create the target for the standalone version of Varkor.
add_executable(varkorStandalone)
target_sources(varkorStandalone PRIVATE src/Main.cc)
target_link_libraries(varkorStandalone varkor)

# Create all of the test targets.
add_subdirectory(src/test)

# Create all of the targets for different Varkor projects.
function(AddProject targetName directory)
  project(${targetName})
  add_executable(${targetName})
  add_subdirectory(${directory}/src ${targetName}Obj)
  target_compile_definitions(${targetName}
    PUBLIC PROJECT_DIRECTORY="${directory}")
  target_link_libraries(${targetName} varkor)
  add_backward(${targetName})
endfunction()

include("projects.cmake" OPTIONAL)
