#include "Material.hpp"

Material::Material()
{
}

void Material::use()
{
    if (shader_program)
        shader_program->use();
}