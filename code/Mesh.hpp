#pragma once

#include <GL/gl.h>

class Mesh
{
protected:

    enum
    {
        COORDINATES_VBO,
        COLORS_VBO,
        INDICES_EBO,
        VBO_COUNT
    };

    GLuint vbo_ids[VBO_COUNT];
    GLuint vao_id = 0;

public:

    virtual ~Mesh();

    void render();
};
