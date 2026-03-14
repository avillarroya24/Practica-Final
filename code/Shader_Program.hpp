#pragma once

// En Windows, siempre incluir windows.h antes de gl.h
#include <windows.h>
#include <GL/gl.h>
#include <string>

class Shader_Program
{
private:
    GLint shader_program_id;   // -1 indica error

    std::string vertex_code;
    std::string fragment_code;

    void compile_shaders();

public:
    // constructor
    Shader_Program(const std::string& vertex_shader_code,
        const std::string& fragment_shader_code);

    // chequea si el shader se compiló y enlazó correctamente
    bool good() const
    {
        return shader_program_id != -1;
    }

    // activa este shader
    void use() const;

    // obtiene la ubicación de un uniform
    GLint get_uniform_location(const char* uniform_name) const
    {
        if (shader_program_id == -1) return -1;
    }
};