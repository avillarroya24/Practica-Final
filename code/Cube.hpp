#ifndef CUBE_HEADER
#define CUBE_HEADER

#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{

    class Cube
    {
    private:

        // Índices para VBOs
        enum
        {
            COORDINATES_VBO,
            COLORS_VBO,
            TEXCOORDS_VBO,
            INDICES_EBO,
            VBO_COUNT
        };

        // Arrays estáticos
        static const GLfloat coordinates[];
        static const GLfloat colors_base[];
        static const GLfloat texcoords[];
        static const GLubyte indices[];

        GLuint vbo_ids[VBO_COUNT];   // IDs de VBOs
        GLuint vao_id;               // ID del VAO

        glm::vec3 vertex_colors[8];  // Colores dinámicos por vértice

        GLuint texture_id;           // ID de la textura OpenGL
        bool use_texture;            // activar/desactivar textura

    public:

        Cube();
        ~Cube();

        // Color dinámico (fallback si no hay textura)
        void set_color(const glm::vec3& factor);

        // Texturas
        void set_texture(GLuint texture);   // asignar textura
        void enable_texture(bool enable);   // activar/desactivar uso de textura

        // Render
        void render();
    };

}

#endif