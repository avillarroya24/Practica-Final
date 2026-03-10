#pragma once

#include <GL/gl.h>
#include <cstddef>  // size_t

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

    GLuint vbo_ids[VBO_COUNT] = { 0 };
    GLuint vao_id = 0;

    GLsizei index_count = 0;

public:

    virtual ~Mesh();

protected:

    void create_buffers(
        const GLfloat* coordinates,
        size_t coordinates_size,
        const GLfloat* colors,
        size_t colors_size,
        const GLubyte* indices,
        size_t indices_size
    );

public:

    void render();
};