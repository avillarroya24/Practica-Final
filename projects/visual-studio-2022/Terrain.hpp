#pragma once
#include "Mesh.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>

class Terrain
{
public:
    Mesh mesh;
    GLuint texture;

    Terrain(int size, float scale, const char* texPath);
};
