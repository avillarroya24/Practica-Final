#ifndef MESH_HPP
#define MESH_HPP

// IMPORTANTE: Asegúrate de que glad se incluye antes de cualquier otra cosa de OpenGL
#include <glad/gl.h>

class Mesh
{
protected:
    // Enumeración para manejar los índices del array de buffers
    enum
    {
        COORDINATES_VBO = 0,
        COLORS_VBO = 1,
        TEXCOORDS_VBO = 2,
        VBO_COUNT = 3
    };

    GLuint vao_id = 0;
    GLuint vbo_ids[VBO_COUNT] = { 0, 0, 0 };
    GLuint ebo_id = 0;

    GLsizei index_count = 0;
    GLsizei vertex_count = 0;

public:
    Mesh() = default;

    // Destructor virtual: indispensable para que Cube limpie sus recursos
    virtual ~Mesh();

    // Renderizado base
    virtual void render() const;
};

#endif