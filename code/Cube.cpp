#include "Cube.hpp"
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

namespace udit
{

    // Vértices del cubo
    const GLfloat Cube::coordinates[] =
    {
       -1,-1,+1,  +1,-1,+1,  +1,+1,+1,  -1,+1,+1,
       -1,-1,-1,  +1,-1,-1,  +1,+1,-1,  -1,+1,-1,
    };

    // Índices para cada cara
    const GLubyte Cube::indices[] =
    {
        0, 1, 2,  0, 2, 3,    // front
        4, 0, 3,  4, 3, 7,    // left
        7, 5, 4,  7, 6, 5,    // back
        1, 5, 6,  1, 6, 2,    // right
        3, 2, 6,  3, 6, 7,    // top
        5, 0, 4,  5, 1, 0     // bottom
    };

    // Colores base por vértice
    const GLfloat Cube::colors_base[] =
    {
        0,0,1,  1,0,1,  1,1,1,  0,1,1,
        0,0,0,  1,0,0,  1,1,0,  0,1,0,
    };

    // Constructor
    Cube::Cube()
    {
        glGenBuffers(VBO_COUNT, vbo_ids);
        glGenVertexArrays(1, &vao_id);

        glBindVertexArray(vao_id);

        // VBO coordenadas
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Inicializar colores dinámicos
        for (int i = 0; i < 8; ++i)
            vertex_colors[i] = glm::vec3(colors_base[i * 3], colors_base[i * 3 + 1], colors_base[i * 3 + 2]);

        // VBO colores
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors_base), colors_base, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // EBO índices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    // Destructor
    Cube::~Cube()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }

    // Actualiza los colores dinámicos por vértice según un factor
    void Cube::set_color(const glm::vec3& factor)
    {
        for (int i = 0; i < 8; ++i)
        {
            vertex_colors[i] = glm::vec3(
                colors_base[i * 3 + 0] * factor.r,
                colors_base[i * 3 + 1] * factor.g,
                colors_base[i * 3 + 2] * factor.b
            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_colors), vertex_colors);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Renderiza el cubo
    void Cube::render()
    {
        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_BYTE, 0);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }

}