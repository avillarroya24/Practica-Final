#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using glm::vec3;
using glm::mat4;

struct Transform
{
    Transform* parent = nullptr;

    vec3 position{ 0.0f, 0.0f, 0.0f };
    vec3 rotation{ 0.0f, 0.0f, 0.0f }; // En grados
    vec3 scale{ 1.0f, 1.0f, 1.0f };

    // Calcula la matriz de transformación local y la combina con la del padre
    mat4 get_transform_matrix() const;
};

#endif