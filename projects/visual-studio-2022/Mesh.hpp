#pragma once
#include <glad/gl.h>

class Mesh
{
public:
    GLuint vao, vbo, ebo;
    GLsizei indexCount;

    Mesh();
    ~Mesh();
    void draw() const;
};
