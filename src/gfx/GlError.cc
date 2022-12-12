#include <string>

#include "Error.h"
#include "gfx/GlError.h"

namespace Gfx {
namespace GlError {

void Init()
{
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(Callback, nullptr);

  // Igore performance and notification messages.
  glDebugMessageControl(
    GL_DONT_CARE,
    GL_DEBUG_TYPE_PERFORMANCE,
    GL_DONT_CARE,
    0,
    nullptr,
    GL_FALSE);
  glDebugMessageControl(
    GL_DONT_CARE,
    GL_DONT_CARE,
    GL_DEBUG_SEVERITY_NOTIFICATION,
    0,
    nullptr,
    GL_FALSE);
}

void LogStatus()
{
  std::string s;
  GLenum error = glGetError();
  switch (error) {
  case GL_NO_ERROR: s += "GL_NO_ERROR"; break;
  case GL_INVALID_ENUM: s += "GL_INVALID_ENUM"; break;
  case GL_INVALID_VALUE: s += "GL_INVALID_VALUE"; break;
  case GL_INVALID_OPERATION: s += "GL_INVALID_OPERATION"; break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    s += "GL_INVALID_FRAMEBUFFER_OPERATION";
    break;
  case GL_OUT_OF_MEMORY: s += "GL_OUT_OF_MEMORY"; break;
  case GL_STACK_UNDERFLOW: s += "GL_STACK_UNDERFLOW"; break;
  case GL_STACK_OVERFLOW: s += "GL_STACK_OVERFLOW"; break;
  }
  Error::LogString(s.c_str());
}

void GLAPIENTRY Callback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam)
{
  std::string e;
  e += "GlError(";
  switch (source) {
  case GL_DEBUG_SOURCE_API: e += "SOURCE_API"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM: e += "SOURCE_WINDOW_SYSTEM"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER: e += "SOURCE_SHADER_COMPILER"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY: e += "SOURCE_THIRD_PARTY"; break;
  case GL_DEBUG_SOURCE_APPLICATION: e += "SOURCE_APPLICATION"; break;
  case GL_DEBUG_SOURCE_OTHER: e += "SOURCE_OTHER"; break;
  }
  e += ", ";
  switch (type) {
  case GL_DEBUG_TYPE_ERROR: e += "TYPE_ERROR"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    e += "TYPE_DEPRECATED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: e += "TYPE_UNDEFINED_BEHAVIOR"; break;
  case GL_DEBUG_TYPE_PORTABILITY: e += "TYPE_PORTABILITY"; break;
  case GL_DEBUG_TYPE_PERFORMANCE: e += "TYPE_PERFORMANCE"; break;
  case GL_DEBUG_TYPE_MARKER: e += "TYPE_MARKER"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP: e += "TYPE_PUSH_GROUP"; break;
  case GL_DEBUG_TYPE_POP_GROUP: e += "TYPE_POP_GROUP"; break;
  case GL_DEBUG_TYPE_OTHER: e += "TYPE_OTHER"; break;
  }
  e += ", ";
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH: e += "SEVERITY_HIGH"; break;
  case GL_DEBUG_SEVERITY_MEDIUM: e += "SEVERITY_MEDIUM"; break;
  case GL_DEBUG_SEVERITY_LOW: e += "SEVERITY_LOW"; break;
  case GL_DEBUG_SEVERITY_NOTIFICATION: e += "SEVERITY_NOTIFICATION"; break;
  }
  e += ")\n  ";
  e += message;
  Error::LogString(e.c_str());
  Error::StackTrace();
}

} // namespace GlError
} // namespace Gfx