#include "Mesh.hpp"

void Mesh::render() const
{
    if (!vao_id) return;

    glBindVertexArray(vao_id);

    if (ebo_id && index_count > 0)
    {
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, 3); // por defecto 3 vértices, se puede sobreescribir
    }

    glBindVertexArray(0);
}