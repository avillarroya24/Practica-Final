#pragma once

#include <string>
#include <GL/glew.h>

using namespace std;

class Shader_Program
{
    GLint shader_program_id = -1;   // -1 indica error

    string vertex_code;
    string fragment_code;

public:

    Shader_Program(const string& vertex_shader_code, const string& fragment_shader_code);

    bool good() const
    {
        return shader_program_id != -1;
    }

    void use();

    GLint get_uniform_location(const char* uniform_name)
    {
        return glGetUniformLocation(shader_program_id, uniform_name);
    }

private:

    void compile_shaders();
};