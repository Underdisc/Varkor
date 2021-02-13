#ifndef gfx_Shader_h
#define gfx_Shader_h

#include <string>

namespace Gfx {

class Shader
{
public:
  struct InitResult
  {
    bool mSuccess;
    std::string mError;
  };

public:
  Shader();
  Shader(const char* vertexFile, const char* fragmentFile);
  InitResult Init(const char* vertexFile, const char* fragmentFile);
  void Use() const;
  unsigned int Id() const;
  bool Live() const;
  int UniformLocation(const char* name) const;
  void SetFloat(const char* name, float value) const;
  void SetVec3(const char* name, const float* data) const;
  void SetVec4(const char* name, const float* data) const;
  void SetMat4(
    const char* name, const float* data, bool transpose = true) const;
  void SetSampler(const char* name, int textureUnit) const;

private:
  InitResult Compile(
    const char* shaderSource, int shaderType, unsigned int* shaderId);

  unsigned int mProgram;

  static constexpr int smInvalidLocation = -1;
};

} // namespace Gfx

#endif
