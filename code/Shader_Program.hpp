#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <glad/gl.h>
#include <GL/gl.h>
#include <string>

class Shader_Program
{
    GLint shader_program_id = -1; // si el id es -1, hubo algún error

    std::string vertex_code;
    std::string fragment_code;

public:

    Shader_Program(const std::string& vertex_shader_code, const std::string& fragment_shader_code);

    bool good() const;

    void use();

    GLint get_uniform_location(const char* uniform_name);

private:

    void compile_shaders();
};

#endif