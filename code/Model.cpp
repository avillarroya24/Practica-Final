#include "Model.hpp"
#include <gtc/type_ptr.hpp>

namespace udit
{
    void Model::render()
    {
        // 1. Verificamos que tengamos lo mínimo para dibujar
        if (!mesh) return;

        // 2. ACTIVAR EL MATERIAL (Shaders y Texturas)
        // Si no hay material, el objeto no se puede dibujar correctamente con shaders
        if (material)
        {
            material->use();

            // 3. OBTENER LA MATRIZ DEL GRAFO
            // transform.get_transform_matrix() ya devuelve la matriz acumulada
            glm::mat4 model_matrix = transform.get_transform_matrix();

            // 4. ENVIAR LA MATRIZ AL SHADER
            // Usamos el nombre exacto de tu shader: "model_view_matrix"
            GLint loc = material->get_shader_program()->get_uniform_location("model_view_matrix");

            if (loc != -1)
            {
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
            }
        }

        // 5. DIBUJAR LA MALLA (VAO Bind y DrawElements)
        mesh->render();
    }

    // Implementación de funciones puente para Scene.cpp
    void Model::set_texture(std::shared_ptr<Texture2D> tex)
    {
        if (material) material->set_texture(tex);
    }

    void Model::enable_texture(bool enable)
    {
        if (material)
        {
            GLint loc = material->get_shader_program()->get_uniform_location("use_texture");
            if (loc != -1) glUniform1i(loc, enable ? 1 : 0);
        }
    }

    void Model::set_color(const glm::vec4& color)
    {
        if (material)
        {
            // Opcional: Si quieres pasar el color al fragment shader
            GLint loc = material->get_shader_program()->get_uniform_location("base_color");
            if (loc != -1) glUniform4fv(loc, 1, glm::value_ptr(color));
        }
    }
}