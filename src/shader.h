#ifndef shader_h
#define shader_h

class Shader
{
public:
  Shader();
  Shader(const char* vertex_file, const char* fragment_file);
  void Init(const char* vertex_file, const char* fragment_file);
  void Use() const;
  unsigned int Id() const;
  int UniformLocation(const char* name) const;
  void SetVec3(const char* name, const float* data) const;
  void SetMat4(
    const char* name, const float* data, bool transpose = false) const;

private:
  bool Compile(
    const char* shader_source, int shader_type, unsigned int* shader_id);

  unsigned int _program;

  static const int _invalid_location;
};

#endif
