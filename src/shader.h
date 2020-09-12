#ifndef shader_h
#define shader_h

class Shader
{
public:
  Shader();
  Shader(const char* vertexFile, const char* fragmentFile);
  void Init(const char* vertexFile, const char* fragmentFile);
  void Use() const;
  unsigned int Id() const;
  int UniformLocation(const char* name) const;
  void SetVec3(const char* name, const float* data) const;
  void SetMat4(
    const char* name, const float* data, bool transpose = false) const;

private:
  bool Compile(
    const char* shaderSource, int shaderType, unsigned int* shaderId);

  unsigned int mProgram;

  static const int smInvalidLocation;
};

#endif
