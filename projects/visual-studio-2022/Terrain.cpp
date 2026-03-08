#include "Terrain.hpp"
#include "Texture.hpp"
#include <vector>
#include <cmath>

Terrain::Terrain(int size, float scale, const char* texPath)
{
    texture = loadTexture2D(texPath);

    std::vector<float> v;
    std::vector<unsigned> idx;

    for (int z = 0; z <= size; z++)
        for (int x = 0; x <= size; x++)
        {
            float xf = float(x) / size;
            float zf = float(z) / size;

            float h = 1.5f * sinf(xf * 6.28f) * cosf(zf * 6.28f);

            v.push_back((xf - 0.5f) * scale);
            v.push_back(h);
            v.push_back((zf - 0.5f) * scale);

            v.push_back(0);
            v.push_back(1);
            v.push_back(0);

            v.push_back(xf * 10);
            v.push_back(zf * 10);
        }

    for (int z = 0; z < size; z++)
        for (int x = 0; x < size; x++)
        {
            int i0 = z * (size + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (size + 1);
            int i3 = i2 + 1;

            idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
            idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
        }

    mesh.indexCount = idx.size();

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned), idx.data(), GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}
