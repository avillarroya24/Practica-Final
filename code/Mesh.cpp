#include "Mesh.hpp"

// Librerías necesarias
#include <GL/gl.h>       // Para GLuint, glGenBuffers, glBindBuffer, glVertexAttribPointer, etc.
#include <cstddef>         // Para size_t
#include <iostream>        // Opcional: para depuración, errores

// Destructor
Mesh::~Mesh()
{
    if (vao_id)
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }
}

// Crear buffers (VBOs y VAO)
void Mesh::create_buffers(
    const GLfloat* coordinates,
    size_t coordinates_size,
    const GLfloat* colors,
    size_t colors_size,
    const GLubyte* indices,
    size_t indices_size
)
{
    index_count = static_cast<GLsizei>(indices_size / sizeof(GLubyte));

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(VBO_COUNT, vbo_ids);

    // Coordenadas
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
    glBufferData(GL_ARRAY_BUFFER, coordinates_size, coordinates, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Colores
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
    glBufferData(GL_ARRAY_BUFFER, colors_size, colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

// Renderizar mesh
void Mesh::render()
{
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_BYTE, 0);
    glBindVertexArray(0);
}