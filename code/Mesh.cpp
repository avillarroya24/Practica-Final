#include "Mesh.hpp"
#include <iostream>

Mesh::~Mesh()
{
    if (vao_id != 0)
        glDeleteVertexArrays(1, &vao_id);

    if (vbo_ids[0] != 0)
        glDeleteBuffers(VBO_COUNT, vbo_ids);

    if (ebo_id != 0)
        glDeleteBuffers(1, &ebo_id);
}

void Mesh::render() const
{
    if (vao_id == 0)
        return;

    glBindVertexArray(vao_id);

    if (ebo_id != 0 && index_count > 0)
    {
        // asegúrate de que el tipo de índices coincide
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_BYTE, nullptr);
    }
    else if (vertex_count > 0)
    {
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }

    glBindVertexArray(0);
}