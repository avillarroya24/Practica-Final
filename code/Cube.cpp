#include "Cube.hpp"
#include <GL/gl.h>

Cube::Cube()
{
    // vértices del cubo
    float coordinates[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    // colores de los vértices
    float colors[] = {
        1,0,0, 1,0,0, 1,0,0, 1,0,0,
        0,1,0, 0,1,0, 0,1,0, 0,1,0
    };

    // índices de los triángulos
    unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        0,1,5, 5,4,0,
        2,3,7, 7,6,2,
        0,3,7, 7,4,0,
        1,2,6, 6,5,1
    };

    // llama a la función de buffers con número de elementos correctos
    create_buffers(
        coordinates, sizeof(coordinates) / sizeof(float),
        colors, sizeof(colors) / sizeof(float),
        indices, sizeof(indices) / sizeof(unsigned int)
    );
}

// implementación mínima de create_buffers para que compile
void Cube::create_buffers(const float* coords, int coords_count,
    const float* colors, int colors_count,
    const unsigned int* indices, int indices_count)
{

}