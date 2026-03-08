#ifndef UDIT_SCENE_HEADER
#define UDIT_SCENE_HEADER

#include <glad/gl.h>
#include <glm.hpp>

#include "Camera.hpp"

namespace udit
{

    class Scene
    {

    private:

        GLuint vao;
        GLuint vbo;

        GLuint shader_program;

        GLuint model_id;
        GLuint view_id;
        GLuint projection_id;

        float earth_rotation = 0.0f;
        float moon_rotation = 0.0f;
        float moon_orbit = 0.0f;

        int vertex_count;

        GLuint compile_shaders();

        void generate_sphere();

    public:

        Scene(int width, int height);

        void update();

        void render();

    };

}

#endif