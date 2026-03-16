#include "Tranform.hpp"
#include <cmath>

mat4 Transform::get_transform_matrix() const
{
    mat4 matrix(1);
    matrix = glm::translate(matrix, position);
    matrix = glm::rotate(matrix, rotation.y, vec3(0, 1, 0));
    matrix = glm::scale(matrix, scale);

    if (parent)
        return parent->get_transform_matrix() * matrix;
    return matrix;
}