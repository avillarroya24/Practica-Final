#include "Material.hpp"
#include "Texture2D.hpp"

Material::Material(std::shared_ptr<Shader_Program> shader)
    : shader_program(shader), texture(nullptr)
{
}

void Material::set_texture(std::shared_ptr<udit::Texture2D> tex)
{
    texture = tex;
}

void Material::use()
{
    if (!shader_program || !shader_program->good())
        return;

    shader_program->use();

    GLint use_tex_loc = shader_program->get_uniform_location("use_texture");

    if (use_tex_loc != -1)
    {
        if (texture)
        {
            texture->bind(0);
            glUniform1i(use_tex_loc, 1);
        }
        else
        {
            glUniform1i(use_tex_loc, 0);
        }
    }
}