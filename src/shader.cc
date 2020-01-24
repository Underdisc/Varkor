#include <glad/glad.h>

#include <fstream>
#include <sstream>

#include "error.h"

#include "shader.h"

Shader::Shader(const char* vertex_file, const char* fragment_file)
{
    unsigned int vert_id, frag_id;
    bool compiled = Compile(vertex_file, GL_VERTEX_SHADER, &vert_id);
    compiled &= Compile(fragment_file, GL_FRAGMENT_SHADER, &frag_id);
    if (!compiled)
    {
        _program = 0;
        return;
    }

    _program = glCreateProgram();
    glAttachShader(_program, vert_id);
    glAttachShader(_program, frag_id);
    glLinkProgram(_program);
    int linked;
    glGetProgramiv(_program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        const int error_log_len = 512;
        char error_log[error_log_len];
        glGetProgramInfoLog(_program, error_log_len, NULL, error_log);
        std::stringstream reason;
        reason << " shader files " << vertex_file << " and " << fragment_file
            << " failed to link." << std::endl << error_log;
        LogError(reason.str().c_str());
        _program = 0;
        return;
    }
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
}

void Shader::Use()
{
    glUseProgram(_program);
}

bool Shader::Compile(
    const char* shader_file,
    int shader_type,
    unsigned int* shader_id)
{
    std::ifstream file;
    file.open(shader_file);
    if (!file.is_open())
    {
        std::stringstream reason;
        reason << "shader file " << shader_file << " does not exist.";
        LogError(reason.str().c_str());
        return false;
    }
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    char* source = new char[length + 1];
    file.read(source, length);
    source[length] = 0;

    *shader_id = glCreateShader(shader_type);
    glShaderSource(*shader_id, 1, &source, NULL);
    glCompileShader(*shader_id);
    int success;
    glGetShaderiv(*shader_id, GL_COMPILE_STATUS, &success);
    if (success)
    {
        return true;
    }

    const int log_len = 512;
    char log[log_len];
    glGetShaderInfoLog(*shader_id, log_len, NULL, log);
    std::stringstream error;
    if (shader_type == GL_VERTEX_SHADER)
    {
        error << "vertex";
    }
    else if (shader_type == GL_FRAGMENT_SHADER)
    {
        error << "fragment";
    }
    error << " shader file " << shader_file << " failed to compile"
        << std::endl << log;
    LogError(error.str().c_str());
    return false;
}