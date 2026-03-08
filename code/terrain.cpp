// TerrainDunes.cpp
#include "Terrain.hpp"
#include <vector>
#include <cmath>

using std::vector;

namespace udit
{
    TerrainDunes::TerrainDunes(float width, float depth, unsigned x_slices, unsigned z_slices)
    {
        generateMesh(width, depth, x_slices, z_slices);
    }

    TerrainDunes::~TerrainDunes()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(1, &vbo_id);
        glDeleteBuffers(1, &ebo_id);
    }

    void TerrainDunes::generateMesh(float width, float depth, unsigned x_slices, unsigned z_slices)
    {
        // -----------------------------
        // VÉRTICES
        // -----------------------------
        vector<float> vertices;
        float x_start = -width * 0.5f;
        float z_start = -depth * 0.5f;
        float x_step = width / float(x_slices);
        float z_step = depth / float(z_slices);

        for (unsigned j = 0; j <= z_slices; ++j)
        {
            float z = z_start + j * z_step;
            for (unsigned i = 0; i <= x_slices; ++i)
            {
                float x = x_start + i * x_step;
                float y = sin(x * 0.5f) * cos(z * 0.7f) * 0.5f; // altura de dunas

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        // -----------------------------
        // ÍNDICES (dos triángulos por celda)
        // -----------------------------
        vector<unsigned int> indices;
        for (unsigned j = 0; j < z_slices; ++j)
        {
            for (unsigned i = 0; i < x_slices; ++i)
            {
                unsigned topLeft = j * (x_slices + 1) + i;
                unsigned topRight = topLeft + 1;
                unsigned bottomLeft = topLeft + (x_slices + 1);
                unsigned bottomRight = bottomLeft + 1;

                // Primer triángulo
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // Segundo triángulo
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        index_count = indices.size();

        // -----------------------------
        // CREAR VAO / VBO / EBO
        // -----------------------------
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(1, &vbo_id);
        glGenBuffers(1, &ebo_id);

        glBindVertexArray(vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void TerrainDunes::render()
    {
        glBindVertexArray(vao_id);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe para ver triángulos
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}