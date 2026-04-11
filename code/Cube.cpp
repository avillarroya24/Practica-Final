#include "Cube.hpp"
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

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
        0, 1, 2,  0, 2, 3,
        4, 0, 3,  4, 3, 7,
        7, 5, 4,  7, 6, 5,
        1, 5, 6,  1, 6, 2,
        3, 2, 6,  3, 6, 7,
        5, 0, 4,  5, 1, 0
    };

    // ================= COLORES =================
    const GLfloat Cube::colors_base[] =
    {
        0,0,1,  1,0,1,  1,1,1,  0,1,1,
        0,0,0,  1,0,0,  1,1,0,  0,1,0,
    };

    // ================= UVs =================
    const GLfloat Cube::texcoords[] =
    {
        0,0,  1,0,  1,1,  0,1,
        0,0,  1,0,  1,1,  0,1
    };

    // ================= CONSTRUCTOR =================
    Cube::Cube()
    {
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(VBO_COUNT, vbo_ids);

        glBindVertexArray(vao_id);

        // ---------------- COORDENADAS ----------------
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // ---------------- COLORES ----------------
        for (int i = 0; i < 8; ++i)
        {
            vertex_colors[i] = glm::vec3(
                colors_base[i * 3 + 0],
                colors_base[i * 3 + 1],
                colors_base[i * 3 + 2]
            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors_base), colors_base, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        // ---------------- UVs ----------------
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORDS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);

        // ---------------- ÍNDICES ----------------
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        // ================= ESTADO TEXTURA =================
        use_texture = false;
        texture_id = 0;

        glEnable(GL_DEPTH_TEST);
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
        for (int i = 0; i < 8; ++i)
        {
            vertex_colors[i] = glm::vec3(
                colors_base[i * 3 + 0] * factor.r,
                colors_base[i * 3 + 1] * factor.g,
                colors_base[i * 3 + 2] * factor.b
            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * 8, vertex_colors);
    }

    // ================= TEXTURAS =================
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

        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_BYTE, 0);

        glBindVertexArray(0);
    }

}