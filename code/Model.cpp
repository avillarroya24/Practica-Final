#include "Model.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

void Model::render(GLint model_view_matrix_id)
{
    glm::mat4 transform_matrix = transform.get_transform_matrix();

    material->use();

    glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(transform_matrix));

    mesh->render();
}
