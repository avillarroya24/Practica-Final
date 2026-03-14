#pragma once

#include <GL/gl.h>

class Mesh
{
protected:

    // enumeración de VBOs
    enum
    {
        COORDINATES_VBO,
        COLORS_VBO,
        INDICES_EBO,
        VBO_COUNT
    };

public:

    // constructor inicializa todo a 0
    Mesh();

    // destructor virtual
    virtual ~Mesh();

    // función de render
    void render();
};