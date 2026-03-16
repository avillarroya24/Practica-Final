#ifndef MESH_HPP
#define MESH_HPP

#include <GL/glew.h>

class Mesh
{
protected:
    GLuint vao_id = 0;
    GLuint vbo_id = 0;

public:
    virtual ~Mesh()
    {
        if (vao_id) glDeleteVertexArrays(1, &vao_id);
        if (vbo_id) glDeleteBuffers(1, &vbo_id);
    }

    virtual void render() const
    {
        glBindVertexArray(vao_id);
        // glDrawArrays o glDrawElements según configuración
        glBindVertexArray(0);
    }
};

#endif