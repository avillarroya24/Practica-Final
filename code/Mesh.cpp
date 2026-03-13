#include "Mesh.hpp"

Mesh::~Mesh()
{
    if (vao_id)
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }
}

void Mesh::render()
{
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
    glBindVertexArray(0);
}
