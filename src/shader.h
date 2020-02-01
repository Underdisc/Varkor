#ifndef Shader_h
#define Shader_h

class Shader
{
public:
    Shader(const char* vertex_file, const char* fragment_file);
    void Use();
    unsigned int Id();
    int UniformLocation(const char* name);
    void SetMat4(const char* name, const float* data);

private:
    bool Compile(
        const char* shader_source,
        int shader_type,
        unsigned int* shader_id);

    unsigned int _program;

    static const int _invalid_location;
};

#endif
