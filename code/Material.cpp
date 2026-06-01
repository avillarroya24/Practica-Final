#include "Material.hpp"
#include "Texture2D.hpp"

/*

   Implementación de la clase Material.
 
 * Este archivo define el comportamiento de un material en el motor gráfico.
 * Un material combina un shader y una textura, y controla cómo se renderiza
   un objeto en pantalla.
*/

// ================= CONSTRUCTOR =================

Material::Material(std::shared_ptr<Shader_Program> shader) //Inicializa el material con un shader asociado y sin textura por defecto.
    : shader_program(shader), texture(nullptr)
{
}

// ================= TEXTURA =================

void Material::set_texture(std::shared_ptr<udit::Texture2D> tex) //Asigna una textura al material.
{
    texture = tex;
}

// ================= USO DEL MATERIAL =================

/*

   Activa el material para su uso en renderizado.

 * Este método:
 * - Activa el shader asociado
 * - Comprueba si existe textura
 * - Activa o desactiva el uso de textura en el shader
 * - Vincula la textura si existe

*/


void Material::use()
{
    if (!shader_program || !shader_program->good()) //Si no hay shader valido, no hace nada
        return;

    shader_program->use(); //Activa el shader

    GLint use_tex_loc = shader_program->get_uniform_location("use_texture"); //Obtiene la ubicacion del uniform

    if (use_tex_loc != -1)
    {
        if (texture) //Si hay textura, se activa y se informa al shader
        {
            texture->bind(0);
            glUniform1i(use_tex_loc, 1);
        }
        else //Si no hay textura, se desactiva en el shader
        {
            glUniform1i(use_tex_loc, 0);
        }
    }
}