#ifndef CUBE_HPP
#define CUBE_HPP

#include "Node.hpp"
#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{
    class Cube : public Node
    {
    private:

        enum
        {
            COORDINATES_VBO,
            COLORS_VBO,
            TEXCOORDS_VBO,
            INDICES_EBO,
            VBO_COUNT
        };

        static const GLfloat coordinates[];
        static const GLubyte indices[];
        static const GLfloat texcoords[];

        GLuint vao_id;
        GLuint vbo_ids[VBO_COUNT];

        GLuint texture_id;
        bool use_texture;

    public:

        Cube();
        ~Cube();

        void render() override;

        void set_color(const glm::vec3& factor);
        void set_texture(GLuint texture);
        void enable_texture(bool enable);
    };
}

#endif