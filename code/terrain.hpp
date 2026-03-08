// TerrainDunes.hpp
#pragma once
#include <glad/gl.h>

namespace udit
{
    class TerrainDunes
    {
    public:
        TerrainDunes(float width, float depth, unsigned x_slices, unsigned z_slices);
        ~TerrainDunes();

        void render();

    private:
        void generateMesh(float width, float depth, unsigned x_slices, unsigned z_slices);

        GLuint vao_id = 0;
        GLuint vbo_id = 0;
        GLuint ebo_id = 0;

        unsigned index_count = 0;
    };
}