#include "Shader_Program.hpp"
#include <iostream>

// Constructor: solo guarda los códigos
Shader_Program::Shader_Program(const std::string& vertex_shader_code,
    const std::string& fragment_shader_code)
    : shader_program_id(-1),
    vertex_code(vertex_shader_code),
    fragment_code(fragment_shader_code)
{
    // no hacemos nada por ahora
}

// compile_shaders vacío (placeholder)
void Shader_Program::compile_shaders()
{
    // no hacemos nada por ahora
}

// activar shader (placeholder)
void Shader_Program::use() const
{
    // no hacemos nada por ahora
}