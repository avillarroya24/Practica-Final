#pragma once
#include <string>
#include <GL/gl.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh
{
protected:
    enum
    {
        COORDINATES_VBO,
        COLORS_VBO,
        INDICES_EBO,
        VBO_COUNT
    };

    GLuint vbo_ids[VBO_COUNT];  // IDs de los VBOs
    GLuint vao_id = 0;          // ID del VAO
    GLuint indices_count = 0;   // Número de índices a dibujar

public:
    Mesh() = default;

    virtual ~Mesh()
    {
        if (vao_id)
        {
            glDeleteVertexArrays(1, &vao_id);
            glDeleteBuffers(VBO_COUNT, vbo_ids);
        }
    }

    // Renderizar la malla
    virtual void render()
    {
        if (vao_id == 0) return;

        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};