#include "Transform.hpp"

mat4 Transform::get_transform_matrix() const
{
    // 1. Creamos la matriz identidad
    mat4 local_matrix = mat4(1.0f);

    // 2. Aplicamos transformaciones locales (Orden: Traslación * Rotación * Escala)
    local_matrix = glm::translate(local_matrix, position);

    // Rotamos en los tres ejes (X, Y, Z) convirtiendo grados a radianes
    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));

    local_matrix = glm::scale(local_matrix, scale);

    // 3. Si hay un padre, multiplicamos su matriz global por nuestra matriz local
    // Esto propaga las transformaciones hacia abajo en el grafo de escena
    if (parent)
    {
        return parent->get_transform_matrix() * local_matrix;
    }

    return local_matrix;
}