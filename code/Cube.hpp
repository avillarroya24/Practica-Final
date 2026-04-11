#ifndef CUBE_HPP
#define CUBE_HPP

#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{
    class Cube
    {
    private:

        // ================= VBO INDICES =================
        enum
        {
            COORDINATES_VBO,
            COLORS_VBO,
            TEXCOORDS_VBO,
            INDICES_EBO,
            VBO_COUNT
        };

        // ================= GEOMETRÍA =================
        static const GLfloat coordinates[];
        static const GLubyte indices[];
        static const GLfloat texcoords[];

        // ================= OPENGL =================
        GLuint vao_id;
        GLuint vbo_ids[VBO_COUNT];

        // ================= MATERIAL =================
        GLuint texture_id;
        bool use_texture;

    public:

        Cube();
        ~Cube();

        // ================= RENDER =================
        void render();

        // ================= COLOR =================
        void set_color(const glm::vec3& factor);

        // ================= TEXTURA =================
        void set_texture(GLuint texture);
        void enable_texture(bool enable);
    };
}

#endif