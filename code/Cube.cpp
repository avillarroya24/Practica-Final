#include "Cube.hpp"
#include <glm.hpp>

/*
    Implementación de la clase Cube.
 * Este archivo define la geometría de un cubo y su comportamiento
 * en renderizado usando OpenGL. Incluye la gestión de buffers,
 * colores dinámicos y texturas sin distorsión.
*/

namespace udit
{
    // ================= VÉRTICES, NORMALES Y UVs (24 VÉRTICES) =================

    // Cada cara tiene sus propios 4 vértices independientes para mapear UVs y normales perfectas.
    const GLfloat Cube::coordinates[] =
    {
        // Cara Frontal
        -1,-1, 1,   1,-1, 1,   1, 1, 1,  -1, 1, 1,
        // Cara Trasera
         1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1,
         // Cara Derecha
          1,-1, 1,   1,-1,-1,   1, 1,-1,   1, 1, 1,
          // Cara Izquierda
          -1,-1,-1,  -1,-1, 1,  -1, 1, 1,  -1, 1,-1,
          // Cara Superior
          -1, 1, 1,   1, 1, 1,   1, 1,-1,  -1, 1,-1,
          // Cara Inferior
          -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1
    };

    // Añadimos normales para que la iluminación funcione impecable en cada cara
    const GLfloat cube_normals[] =
    {
        // Frontal
         0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,
         // Trasera
          0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1,
          // Derecha
           1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,
           // Izquierda
           -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,
           // Superior
            0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,
            // Inferior
             0,-1, 0,  -0,-1, 0,  -0,-1, 0,  -0,-1, 0
    };

    const GLfloat Cube::texcoords[] =
    {
        // Cara frontal
        0,0,  1,0,  1,1,  0,1,
        // Cara trasera
        0,0,  1,0,  1,1,  0,1,
        // Cara derecha
        0,0,  1,0,  1,1,  0,1,
        // Cara izquierda
        0,0,  1,0,  1,1,  0,1,
        // Cara superior
        0,0,  1,0,  1,1,  0,1,
        // Cara inferior
        0,0,  1,0,  1,1,  0,1
    };

    // ================= ÍNDICES ACTUALIZADOS =================

    const GLubyte Cube::indices[] =
    {
         0, 1, 2,   0, 2, 3,    // Frontal
         4, 5, 6,   4, 6, 7,    // Trasera
         8, 9,10,   8,10,11,    // Derecha
        12,13,14,  12,14,15,    // Izquierda
        16,17,18,  16,18,19,    // Superior
        20,21,22,  20,22,23     // Inferior
    };

    // ================= CONSTRUCTOR =================

    Cube::Cube()
    {
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(VBO_COUNT, vbo_ids);

        glBindVertexArray(vao_id);

        // 1. POSICIONES (Layout 0)
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // 2. NORMALES (Layout 1) - Reutilizamos o mapeamos dinámicamente si tu enum lo permite, 
        // aquí lo configuramos directo al layout 1 del Shader de la Escena.
        GLuint normal_vbo;
        glGenBuffers(1, &normal_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        // 3. UVs (Layout 2)
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORDS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        // 4. COLORES (Se adaptan a los nuevos 24 vértices)
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24 * 3, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Mapeado opcional
        glEnableVertexAttribArray(3);

        // 5. ÍNDICES
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        use_texture = false;
        texture_id = 0;
    }

    // ================= DESTRUCTOR =================

    Cube::~Cube()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }

    // ================= COLOR DINÁMICO (ACTUALIZADO A 24 VÉRTICES) =================

    void Cube::set_color(const glm::vec3& color)
    {
        GLfloat data[24 * 3]; // 24 vértices totales

        for (int i = 0; i < 24; i++)
        {
            data[i * 3 + 0] = color.r;
            data[i * 3 + 1] = color.g;
            data[i * 3 + 2] = color.b;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
    }

    // ================= TEXTURA =================

    void Cube::set_texture(GLuint texture)
    {
        texture_id = texture;
    }

    void Cube::enable_texture(bool enable)
    {
        use_texture = enable;
    }

    // ================= RENDER =================

    void Cube::render()
    {
        glBindVertexArray(vao_id);

        if (use_texture && texture_id != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Se siguen dibujando 36 índices (6 caras * 2 triángulos * 3 vértices)
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);

        glBindVertexArray(0);

        for (auto* child : children)
            if (child)
                child->render();
    }
}