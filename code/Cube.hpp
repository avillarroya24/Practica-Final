#ifndef CUBE_HEADER
#define CUBE_HEADER

#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{

    class Cube
    {
    private:

        // Índices para indexar el array vbo_ids
        enum
        {
            COORDINATES_VBO,
            COLORS_VBO,
            INDICES_EBO,
            VBO_COUNT
        };

        // Arrays estáticos de vértices, colores base e índices
        static const GLfloat coordinates[];
        static const GLfloat colors_base[];
        static const GLubyte indices[];

        GLuint vbo_ids[VBO_COUNT];   // IDs de VBOs
        GLuint vao_id;               // ID del VAO

        glm::vec3 vertex_colors[8];  // Colores dinámicos por vértice

    public:

        Cube();
        ~Cube();

        void set_color(const glm::vec3& factor); // Para actualizar colores dinámicamente
        void render();                            // Dibuja el cubo

    };

}

#endif