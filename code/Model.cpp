#include "Model.hpp"
#include <gtc/type_ptr.hpp>

/*

   Implementación de la clase Model.

 * Este archivo define el comportamiento de un modelo 3D dentro del motor.
 * Un modelo combina una malla (Mesh), un material (Material) y una transformación
   para poder ser renderizado correctamente en la escena.
*/

namespace udit
{
    // ================= RENDER =================

    /*
    
       Renderiza el modelo.
     
     * Este método:
     * - Comprueba que exista una malla válida
     * - Activa el material (shader + textura)
     * - Calcula y envía la matriz de transformación al shader
     * - Llama al render de la malla
    
    */

    void Model::render()
    {
        if (!mesh)
            return;

        if (material)
        {
            material->use();

            glm::mat4 model_matrix = transform.get_transform_matrix(); //Matriz de trasformacion del modelo

            GLint loc = material->get_shader_program() //Se obtiene la ubicacion del uniform shader
                ->get_uniform_location("model_view_matrix");

            if (loc != -1) //Se envia la amtriz al shader si existe el uniform
            {
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
            }
        }

        mesh->render(); //Render de la geometria
    }


    // ================= TEXTURA =================

    /*
    
       Asigna una textura al modelo.
     
     * La textura se delega al material asociado.
     
     * tex Textura 2D que se aplicará al modelo.
    */

    void Model::set_texture(std::shared_ptr<Texture2D> tex)
    {
        if (material)
            material->set_texture(tex);
    }

    // ================= COLOR =================

    /*
    
       Establece el color base del modelo.
     
     * Envía un color uniforme al shader si el material está disponible.
     
     * color Color RGBA del modelo.
    
    */

    void Model::set_color(const glm::vec4& color)
    {
        if (!material)
            return;

        GLint loc = material->get_shader_program() //Se obtiene el uniform base color del shader
            ->get_uniform_location("base_color");

        if (loc != -1) //Se envia el color al shader si existe
        {
            glUniform4fv(loc, 1, glm::value_ptr(color));
        }
    }

    void Model::enable_texture(bool enable)
    {
        if (!material)
            return;

        GLint loc = material->get_shader_program()
            ->get_uniform_location("use_texture");

        if (loc != -1)
        {
            glUniform1i(loc, enable ? 1 : 0);
        }
    }

}