// The following two includes must always be in this order. There is a macro in
// glad/glad.h that will check if __gl_h_ is already defined. This will cause a
// compiler error if it is. The macro exists to prevent a situation in which
// multiple OpenGL headers are included. glad/glad.h must be included first
// because GLFW/glfw3.h will include the OpenGL header distributed with the
// operating system and glad/glad.h will handle declarations for later versions
// of OpenGL, whereas the os header might not. The Windows header, for example,
// only handles OpenGL 1.1.
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
