#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
    Transform* parent = nullptr;

    glm::vec3 position{ 0,0,0 };
    glm::vec3 rotation{ 0,0,0 };
    glm::vec3 scale{ 1,1,1 };

    glm::mat4 get_transform_matrix() const
    {
        glm::mat4 transform_matrix(1.0f);

        transform_matrix = glm::translate(transform_matrix, position);

        transform_matrix = glm::rotate(transform_matrix, rotation.x, glm::vec3(1, 0, 0));
        transform_matrix = glm::rotate(transform_matrix, rotation.y, glm::vec3(0, 1, 0));
        transform_matrix = glm::rotate(transform_matrix, rotation.z, glm::vec3(0, 0, 1));

        transform_matrix = glm::scale(transform_matrix, scale);

        if (parent)
            return parent->get_transform_matrix() * transform_matrix;

        return transform_matrix;
    }
};
