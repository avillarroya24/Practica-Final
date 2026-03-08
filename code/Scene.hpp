#pragma once
#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

namespace udit
{
    class Scene
    {
    public:
        Scene(int width, int height);
        ~Scene();

        void update();
        void render();

    private:
        void generate_sphere();
        GLuint compile_shaders();

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;   //NECESARIO PARA GL_LINES

        GLuint shader_program = 0;

        GLint model_id = -1;
        GLint view_id = -1;
        GLint projection_id = -1;

        unsigned vertex_count = 0;

        float earth_rotation = 0.0f;
        float moon_rotation = 0.0f;
        float moon_orbit = 0.0f;

        int width, height;
    };
}
