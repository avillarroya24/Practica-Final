#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
    Transform* parent = nullptr;

    glm::vec3 position{ 0.f,0.f,0.f };
    glm::vec3 rotation{ 0.f,0.f,0.f };
    glm::vec3 scale{ 1.f,1.f,1.f };

    glm::mat4 get_transform_matrix() const
    {
        glm::mat4 matrix(1.f);

        matrix = glm::translate(matrix, position);

        matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
        matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
        matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

        matrix = glm::scale(matrix, scale);

        if (parent)
            return parent->get_transform_matrix() * matrix;

        return matrix;
    }
};