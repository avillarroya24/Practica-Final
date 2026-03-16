#include "Mesh.hpp"
#include <GL/glew.h>

Mesh::~Mesh()
{
    if (vao_id) glDeleteVertexArrays(1, &vao_id);
    if (vbo_id) glDeleteBuffers(1, &vbo_id);
}

void Mesh::render() const
{
    if (!vao_id) return;
    glBindVertexArray(vao_id);
    // glDrawArrays o glDrawElements según configuración
    glBindVertexArray(0);
}