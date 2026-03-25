#ifndef MESH_HPP
#define MESH_HPP

#include <glad/gl.h>

class Mesh
{
protected:
    GLuint vao_id = 0;
    GLuint vbo_id = 0;
    GLuint ebo_id = 0; // Añadido para índices

    GLsizei index_count = 0; // Número de índices para DrawElements

public:
    virtual ~Mesh()
    {
        if (vao_id) glDeleteVertexArrays(1, &vao_id);
        if (vbo_id) glDeleteBuffers(1, &vbo_id);
        if (ebo_id) glDeleteBuffers(1, &ebo_id);
    }

    virtual void render() const;
};

#endif