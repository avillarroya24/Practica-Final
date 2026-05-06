#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

using glm::vec3;
using glm::mat4;

struct Transform
{
    Transform* parent = nullptr;

    vec3 position{ 0.0f, 0.0f, 0.0f };
    vec3 rotation{ 0.0f, 0.0f, 0.0f }; // grados
    vec3 scale{ 1.0f, 1.0f, 1.0f };

    mat4 get_transform_matrix() const;
};

#endif