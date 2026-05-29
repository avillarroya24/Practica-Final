#include "Model.hpp"
#include <gtc/type_ptr.hpp>

namespace udit
{
    void Model::render()
    {
        if (!mesh)
            return;

        if (material)
        {
            material->use();

            glm::mat4 model_matrix = transform.get_transform_matrix();

            GLint loc = material->get_shader_program()
                ->get_uniform_location("model_view_matrix");

            if (loc != -1)
            {
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
            }
        }

        mesh->render();
    }

    void Model::set_texture(std::shared_ptr<Texture2D> tex)
    {
        if (material)
            material->set_texture(tex);
    }

    void Model::set_color(const glm::vec4& color)
    {
        if (!material)
            return;

        GLint loc = material->get_shader_program()
            ->get_uniform_location("base_color");

        if (loc != -1)
        {
            glUniform4fv(loc, 1, glm::value_ptr(color));
        }
    }
}