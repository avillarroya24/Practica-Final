#include "Material.hpp"
#include "Shader_Program.hpp"

void Material::use()
{
    shader_program->use();
}
