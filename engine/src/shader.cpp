#include <assert.h>
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <memory>

#include <errors.hpp>
#include <shader.hpp>

namespace nano
{

class shader
{
};

int
create_shader(shader_t type, const char* src, char* compile_info, uint buf_size)
{
    GLuint shader = glCreateShader(static_cast<GLenum>(type));
    OM_GL_CHECK();
    glShaderSource(shader, 1, &src, nullptr);
    OM_GL_CHECK();
    glCompileShader(shader);
    OM_GL_CHECK();

    GLint exit_code{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &exit_code);
    OM_GL_CHECK();
    if (GL_FALSE == exit_code)
    {
        glGetShaderInfoLog(shader, buf_size, nullptr, compile_info);
        OM_GL_CHECK();
        glDeleteShader(shader);
        OM_GL_CHECK();
        return -1;
    }
    return shader;
}

// TERRRRRRIBLE
int
setup_shaders(const char* vertex_shader_src_path,
              const char* fragment_shader_src_path)
{
    auto deleter = [](auto ptr)
    {
        if (ptr)
            delete[] ptr;
    };
    std::shared_ptr<char> vertex_shader_src, fragment_shader_src;
    {
        std::ifstream vertex_shader_stream(vertex_shader_src_path);
        std::ifstream fragment_shader_stream(fragment_shader_src_path);
        size_t vertex_shader_src_size =
                   std::filesystem::file_size(vertex_shader_src_path),
               fragment_shader_src_size =
                   std::filesystem::file_size(fragment_shader_src_path);

        vertex_shader_src =
            std::shared_ptr<char>(new char[vertex_shader_src_size]{}, deleter);
        fragment_shader_src = std::shared_ptr<char>(
            new char[fragment_shader_src_size]{}, deleter);

        vertex_shader_stream.read(vertex_shader_src.get(),
                                  vertex_shader_src_size);
        fragment_shader_stream.read(fragment_shader_src.get(),
                                    fragment_shader_src_size);
    }

    char compile_info[256];
    GLuint vertex_shader =
        create_shader(static_cast<shader_t>(GL_VERTEX_SHADER),
                      vertex_shader_src.get(),
                      compile_info,
                      256);
    if (-1 == vertex_shader)
    {
        std::cerr << compile_info;
        return -1;
    }
    GLuint fragment_shader =
        create_shader(static_cast<shader_t>(GL_FRAGMENT_SHADER),
                      fragment_shader_src.get(),
                      compile_info,
                      256);
    if (-1 == fragment_shader)
    {
        std::cerr << compile_info;
        return -1;
    }

    GLuint program = glCreateProgram();
    OM_GL_CHECK();
    assert(0 != program && "Failed to create GLES program");

    glAttachShader(program, vertex_shader);
    OM_GL_CHECK();
    glAttachShader(program, fragment_shader);
    OM_GL_CHECK();

    glLinkProgram(program);
    OM_GL_CHECK();
    GLint exit_code = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &exit_code);
    OM_GL_CHECK();
    if (GL_FALSE == exit_code)
    {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        OM_GL_CHECK();
        std::string info;
        info.resize(len);
        glGetProgramInfoLog(program, len, nullptr, info.data());
        OM_GL_CHECK();
        std::cerr << info;
        glDeleteProgram(program);
        OM_GL_CHECK();
        return -1;
    }

    return program;
}

} // namespace nano
