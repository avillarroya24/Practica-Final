#include "Transform.hpp"

mat4 Transform::get_transform_matrix() const
{
    mat4 model = mat4(1.0f);

    // 1. Traslación
    model = glm::translate(model, position);

    // 2. Rotación (IMPORTANTE: convertir a radianes)
    model = glm::rotate(model, glm::radians(rotation.x), vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), vec3(0, 0, 1));

    // 3. Escala
    model = glm::scale(model, scale);

    // 4. Jerarquía (grafo de escena)
    if (parent)
        return parent->get_transform_matrix() * model;

    return model;
}