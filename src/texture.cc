#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb/stb_image.h>

#include "error.h"
#include "texture.h"

Texture::Texture(const char* texture_file) :
    _width(0),
    _height(0),
    _channels(0)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(
        texture_file,
        &_width,
        &_height,
        &_channels,
        0);
    if(!data)
    {
        LogError(stbi_failure_reason());
        _id = 0;
        return;
    }

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _width,
        _height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

Texture::~Texture()
{
}

int Texture::Width() const
{
    return _width;
}

int Texture::Height() const
{
    return _height;
}

int Texture::Channels() const
{
    return _channels;
}

unsigned int Texture::Id() const
{
    return _id;
}
