#include "Shader_Program.hpp"
#include <iostream>

using namespace std;

Shader_Program::Shader_Program(const string& vertex_shader_code, const string& fragment_shader_code)
    : vertex_code(vertex_shader_code), fragment_code(fragment_shader_code)
{
    compile_shaders();
}

bool Shader_Program::good() const
{
    return shader_program_id != -1;
}

void Shader_Program::use()
{
    if (good())
        glUseProgram(shader_program_id);
}

GLint Shader_Program::get_uniform_location(const char* uniform_name)
{
    return glGetUniformLocation(shader_program_id, uniform_name);
}

void Shader_Program::compile_shaders()
{
    // --- Vertex Shader ---
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* v_code = vertex_code.c_str();
    glShaderSource(vertex_shader, 1, &v_code, nullptr);
    glCompileShader(vertex_shader);

    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        cout << "Error compiling vertex shader:\n" << infoLog << endl;
        shader_program_id = -1;
        return;
    }

    // --- Fragment Shader ---
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* f_code = fragment_code.c_str();
    glShaderSource(fragment_shader, 1, &f_code, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        cout << "Error compiling fragment shader:\n" << infoLog << endl;
        shader_program_id = -1;
        return;
    }

    // --- Program ---
    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader);
    glAttachShader(shader_program_id, fragment_shader);
    glLinkProgram(shader_program_id);

    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shader_program_id, 512, nullptr, infoLog);
        cout << "Error linking shader program:\n" << infoLog << endl;
        shader_program_id = -1;
    }

    // Limpiar shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}