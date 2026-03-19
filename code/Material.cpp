#include "Material.hpp"

Material::Material(std::shared_ptr<Shader_Program> shader)
    : shader_program(shader)
{
}

void Material::use()
{
    if (shader_program && shader_program->good())
    {
        shader_program->use();
    }
}