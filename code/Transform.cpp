#include "Transform.hpp"

glm::mat4 Transform::get_transform_matrix() const
{
    glm::mat4 local_matrix(1.0f);

    local_matrix = glm::translate(local_matrix, position);

    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    local_matrix = glm::scale(local_matrix, scale);

    return parent
        ? parent->get_transform_matrix() * local_matrix
        : local_matrix;
}