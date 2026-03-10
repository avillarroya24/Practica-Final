#include "Model.hpp"

Model::Model()
{
}

void Model::render(GLint model_view_matrix_id)
{
    glm::mat4 matrix = transform.get_transform_matrix();

    material->use();

    glUniformMatrix4fv(
        model_view_matrix_id,
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );

    mesh->render();
}