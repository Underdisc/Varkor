#ifndef Shader_h
#define Shader_h

class Shader
{
public:
    Shader(const char* vertex_file, const char* fragment_file);
    void Use();
private:
    bool Compile(
        const char* shader_source,
        int shader_type,
        unsigned int* shader_id);

    unsigned int _program;
};

#endif
