#include "Shader_Program.hpp"
#include <iostream>

Shader_Program::Shader_Program(const string& vertex_shader_code, const string& fragment_shader_code)
    : vertex_code(vertex_shader_code), fragment_code(fragment_shader_code)
{
    compile_shaders();
}

void Shader_Program::compile_shaders()
{
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* v_code = vertex_code.c_str();
    glShaderSource(vertex_shader, 1, &v_code, nullptr);
    glCompileShader(vertex_shader);

    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info);
        std::cerr << "Vertex shader error:\n" << info << std::endl;
        shader_program_id = -1;
        return;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* f_code = fragment_code.c_str();
    glShaderSource(fragment_shader, 1, &f_code, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info);
        std::cerr << "Fragment shader error:\n" << info << std::endl;
        shader_program_id = -1;
        return;
    }

    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader);
    glAttachShader(shader_program_id, fragment_shader);
    glLinkProgram(shader_program_id);

    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetProgramInfoLog(shader_program_id, 512, nullptr, info);
        std::cerr << "Shader program link error:\n" << info << std::endl;
        shader_program_id = -1;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader_Program::use()
{
    if (shader_program_id != -1)
        glUseProgram(shader_program_id);
}