#include "Mesh.hpp"
#include <iostream>

Mesh::~Mesh()
{
    // Limpieza segura de recursos en la GPU
    if (vao_id != 0) glDeleteVertexArrays(1, &vao_id);
    if (vbo_ids[0] != 0) glDeleteBuffers(VBO_COUNT, vbo_ids);
    if (ebo_id != 0) glDeleteBuffers(1, &ebo_id);
}

void Mesh::render() const
{
    // Si no hay VAO, no hay nada que dibujar
    if (vao_id == 0) return;

    glBindVertexArray(vao_id);

    // Prioridad 1: Dibujar por índices (EBO) - Es lo que usa tu Cube
    if (ebo_id != 0 && index_count > 0)
    {
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_BYTE, nullptr);
        // NOTA: Si tus índices son GLuint, cambia GL_UNSIGNED_BYTE por GL_UNSIGNED_INT
    }
    // Prioridad 2: Dibujar por arreglos de vértices (VBO)
    else if (vertex_count > 0)
    {
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }

    glBindVertexArray(0);
}