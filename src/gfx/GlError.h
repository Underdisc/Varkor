#include <glad/glad.h>

namespace Gfx {
namespace GlError {

void Init();
void LogFramebufferStatus(GLenum target);
void LogStatus();
void GLAPIENTRY Callback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam);

} // namespace GlError
} // namespace Gfx
