#include "vec.hpp"
#include <errors.hpp>
#include <glad/glad.h>
#include <texture.hpp>

namespace nano
{

texture::texture(const canvas& img)
{
    load(img);
}

texture::~texture()
{
    glDeleteTextures(1, &handle);
}

vec2s
texture::size() const
{
    return m_size;
}

std::uint32_t
texture::get_handle() const
{
    return handle;
}

void
texture::set_size(vec2s new_size)
{
    m_size = new_size;
}

int
texture::load(const canvas& image)
{
    glGenTextures(1, &handle);
    OM_GL_CHECK();
    glBindTexture(GL_TEXTURE_2D, handle);
    OM_GL_CHECK();

    set_size(image.size());
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 size().fst,
                 size().snd,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 &image(0, 0));
    OM_GL_CHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OM_GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OM_GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    OM_GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    OM_GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    OM_GL_CHECK();

    return EXIT_SUCCESS;
}

} // namespace nano
