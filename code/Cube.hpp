#pragma once

class Cube
{
public:
    Cube();

    // función mínima que crea buffers (puedes implementarla más tarde con OpenGL)
    void create_buffers(const float* coords, int coords_count,
        const float* colors, int colors_count,
        const unsigned int* indices, int indices_count);
};