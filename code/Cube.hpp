#pragma once

#include "Mesh.hpp"

class Cube : public Mesh
{
    static const GLfloat coordinates[];
    static const GLfloat colors[];
    static const GLubyte indices[];

public:

    Cube();
};