#include "Material.hpp"
#include "Texture2D.hpp" // Para poder usar bind()

Material::Material(std::shared_ptr<Shader_Program> shader)
    : shader_program(shader), texture(nullptr) // Inicializamos sin textura
{
}

void Material::set_texture(std::shared_ptr<udit::Texture2D> tex)
{
    texture = tex;
}

void Material::use()
{
    if (shader_program && shader_program->good())
    {
        shader_program->use();

        // Si el material tiene textura, la activamos
        if (texture)
        {
            // La vinculamos al slot 0 (coincide con el sampler del shader)
            texture->bind(0);

            // Avisamos al shader que use la textura
            GLint use_tex_loc = shader_program->get_uniform_location("use_texture");
            if (use_tex_loc != -1) glUniform1i(use_tex_loc, 1);
        }
        else
        {
            // Si no hay textura, avisamos al shader para que use color plano
            GLint use_tex_loc = shader_program->get_uniform_location("use_texture");
            if (use_tex_loc != -1) glUniform1i(use_tex_loc, 0);
        }
    }
}