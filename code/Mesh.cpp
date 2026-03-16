#include "Mesh.hpp"

void Mesh::render() const
{
    if (!vao_id) return;
    glBindVertexArray(vao_id);
    // glDrawArrays o glDrawElements según configuración
    glBindVertexArray(0);
}