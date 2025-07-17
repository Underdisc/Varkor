cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(varkor)

# Create the target for the Varkor static library.
add_library(varkor STATIC)
add_subdirectory(ext)
add_subdirectory(src)
add_externals(varkor)

find_package(OpenGL)
target_link_libraries(varkor ${OPENGL_gl_LIBRARY})
target_include_directories(varkor PUBLIC "src")
target_compile_definitions(varkor
  PUBLIC VARKOR_WORKING_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/working/")

# Set compiler options.
target_compile_features(varkor PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(varkor PRIVATE /WX /W3 /sdl /permissive-)
  if("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    target_link_options(varkor PRIVATE /MANIFEST:NO)
  endif()
endif()
