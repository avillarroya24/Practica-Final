#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace udit
{
    class Scene
    {
    private:
        int width;
        int height;

        float bigRotation;
        float smallOrbit;
        float smallRotation;

        GLuint vao;
        GLuint vbo;

        int vertexCount;

    public:
        Scene(int width, int height);

        void update();
        void render();
        void resize(int new_width, int new_height);
    };
}
