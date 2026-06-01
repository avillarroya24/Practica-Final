#include "Mesh.hpp"
#include <iostream>

/*

   Implementación de la clase Mesh.
 
 * Este archivo gestiona la representación geométrica de una malla 3D,
   incluyendo la liberación de recursos de OpenGL y el renderizado
   mediante VAO, VBO y EBO.

*/

// ================= DESTRUCTOR =================

Mesh::~Mesh()
{
    if (vao_id != 0)
        glDeleteVertexArrays(1, &vao_id);

    if (vbo_ids[0] != 0)
        glDeleteBuffers(VBO_COUNT, vbo_ids);

    if (ebo_id != 0)
        glDeleteBuffers(1, &ebo_id);
}

// ================= RENDER =================

/*

   Renderiza la malla.

 * Dibuja la geometría asociada a la malla utilizando OpenGL.
 * Dependiendo de si existe EBO o no, usa glDrawElements o glDrawArrays.

*/

void Mesh::render() const
{
    if (vao_id == 0)
        return;

    glBindVertexArray(vao_id); //Se enlaza el VAO

    if (ebo_id != 0 && index_count > 0) //Render con indices EBO
    {
        // asegúrate de que el tipo de índices coincide
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_BYTE, nullptr);
    }
    else if (vertex_count > 0) //Render sin indices
    {
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }

    glBindVertexArray(0); //Se desactiva el VAO
}