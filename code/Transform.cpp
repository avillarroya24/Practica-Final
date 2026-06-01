#include "Transform.hpp"

/*

   Implementación de la clase Transform.
 
 * Este módulo gestiona la transformación espacial de los objetos en la escena.
 * Incluye posición, rotación, escala y soporte para jerarquía de transformaciones
 * mediante relaciones padre-hijo.

*/


// ================= MATRIZ DE TRANSFORMACIÓN =================

/*

   Calcula la matriz de transformación del objeto.
 
 * Construye la matriz local aplicando:
 * - Traslación
 * - Rotación en los ejes X, Y, Z
 * - Escalado
 
 * Si el objeto tiene un padre, combina su transformación con la del padre
 * para soportar jerarquías (scene graph).
 
 * @return Matriz 4x4 de transformación en espacio mundo.

*/


// ================= TRASLACIÓN =================

glm::mat4 Transform::get_transform_matrix() const
{
    glm::mat4 local_matrix(1.0f);

    local_matrix = glm::translate(local_matrix, position);

    // ================= ROTACIÓN =================

    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    local_matrix = glm::rotate(local_matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // ================= ESCALADO =================

    local_matrix = glm::scale(local_matrix, scale);

    // ================= JERARQUÍA (PARENTING) =================

    return parent
        ? parent->get_transform_matrix() * local_matrix
        : local_matrix;
}