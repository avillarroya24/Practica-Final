#include "Cube.hpp"
#include <glm.hpp>

namespace udit
{
    // ================= VÉRTICES =================
    const GLfloat Cube::coordinates[] =
    {
       -1,-1,+1,  +1,-1,+1,  +1,+1,+1,  -1,+1,+1,
       -1,-1,-1,  +1,-1,-1,  +1,+1,-1,  -1,+1,-1,
    };

    // ================= ÍNDICES =================
    const GLubyte Cube::indices[] =
    {
        0,1,2, 0,2,3,
        1,5,6, 1,6,2,
        5,4,7, 5,7,6,
        4,0,3, 4,3,7,
        3,2,6, 3,6,7,
        4,5,1, 4,1,0
    };

    // ================= UVs =================
    const GLfloat Cube::texcoords[] =
    {
        0,0, 1,0, 1,1, 0,1,
        0,0, 1,0, 1,1, 0,1
    };

    // ================= CONSTRUCTOR =================
    Cube::Cube()
    {
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(VBO_COUNT, vbo_ids);

        glBindVertexArray(vao_id);

        // -------- POSICIONES --------
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // -------- COLORES --------
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        // -------- UVs --------
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORDS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        // -------- ÍNDICES --------
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        // estado inicial
        use_texture = false;
        texture_id = 0;
    }

    // ================= DESTRUCTOR =================
    Cube::~Cube()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }

    // ================= COLOR DINÁMICO =================
    void Cube::set_color(const glm::vec3& factor)
    {
        GLfloat data[24];

        for (int i = 0; i < 8; i++)
        {
            data[i * 3 + 0] = factor.r;
            data[i * 3 + 1] = factor.g;
            data[i * 3 + 2] = factor.b;
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

        if (use_texture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
        }

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);

        glBindVertexArray(0);
    }

}