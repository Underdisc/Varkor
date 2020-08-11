cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)
project(varkor VERSION 0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../working")

add_executable(varkor)
add_subdirectory(src)
target_sources(varkor PRIVATE "ext/src/glad.c")

find_package(OpenGL)

list(APPEND CMAKE_PREFIX_PATH "ext/lib")
find_library(glfw NAMES glfw_mingw32 REQUIRED)

target_include_directories(varkor PRIVATE "ext/inc" "ext/sub")
target_link_libraries(varkor ${glfw} ${OPENGL_gl_LIBRARY})

