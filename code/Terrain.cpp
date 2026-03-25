#include "Terrain.hpp"
#include <cmath>
#include <iostream>
#include <random>

Terrain::Terrain(int width, int height, float scale)
    : m_width(width), m_height(height), m_scale(scale)
{
    GenerateHeightmap();
    BuildMesh();
    UploadToGPU();
}

Terrain::~Terrain()
{
    if (vao_id) glDeleteVertexArrays(1, &vao_id);
    if (vbo_id) glDeleteBuffers(1, &vbo_id);
    if (ebo_id) glDeleteBuffers(1, &ebo_id);
}

void Terrain::Draw() const
{
    if (!vao_id) return;
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Genera alturas mas altas y con un poco de ruido
void Terrain::GenerateHeightmap()
{
    m_heights.resize(m_width * m_height);

    // Generador de ruido aleatorio pequeño
    std::mt19937 rng(42); // semilla fija
    std::uniform_real_distribution<float> noise(-0.2f, 0.2f);

    for (int z = 0; z < m_height; ++z)
    {
        for (int x = 0; x < m_width; ++x)
        {
            // Elevación mas marcada
            float height = (std::sin(x * 0.1f) * std::cos(z * 0.2f) * 7.0f + noise(rng)) * m_scale;
            m_heights[z * m_width + x] = height;
        }
    }
}

float Terrain::GetHeight(int x, int z) const
{
    return m_heights[z * m_width + x];
}

// Calcula normales
glm::vec3 Terrain::CalculateNormal(int x, int z) const
{
    float hL = (x > 0) ? GetHeight(x - 1, z) : GetHeight(x, z);
    float hR = (x < m_width - 1) ? GetHeight(x + 1, z) : GetHeight(x, z);
    float hD = (z > 0) ? GetHeight(x, z - 1) : GetHeight(x, z);
    float hU = (z < m_height - 1) ? GetHeight(x, z + 1) : GetHeight(x, z);

    glm::vec3 normal(hL - hR, 2.0f, hD - hU);
    return glm::normalize(normal);
}

// Construye vertices e indices
void Terrain::BuildMesh()
{
    m_vertices.clear();
    m_indices.clear();

    for (int z = 0; z < m_height; ++z)
    {
        for (int x = 0; x < m_width; ++x)
        {
            float y = GetHeight(x, z);
            glm::vec3 pos(x * m_scale, y, z * m_scale);
            glm::vec3 normal = CalculateNormal(x, z);
            glm::vec2 uv(float(x) / m_width, float(z) / m_height);
            m_vertices.emplace_back(pos, normal, uv);
        }
    }

    for (int z = 0; z < m_height - 1; ++z)
    {
        for (int x = 0; x < m_width - 1; ++x)
        {
            int topLeft = z * m_width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * m_width + x;
            int bottomRight = bottomLeft + 1;

            m_indices.push_back(topLeft);
            m_indices.push_back(bottomLeft);
            m_indices.push_back(topRight);

            m_indices.push_back(topRight);
            m_indices.push_back(bottomLeft);
            m_indices.push_back(bottomRight);
        }
    }
}

// Sube la malla a GPU
void Terrain::UploadToGPU()
{
    std::vector<float> vertex_data;
    for (const auto& v : m_vertices)
    {
        vertex_data.push_back(v.position.x);
        vertex_data.push_back(v.position.y);
        vertex_data.push_back(v.position.z);

        vertex_data.push_back(v.normal.x);
        vertex_data.push_back(v.normal.y);
        vertex_data.push_back(v.normal.z);

        vertex_data.push_back(v.texCoords.x);
        vertex_data.push_back(v.texCoords.y);
    }

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    index_count = static_cast<GLsizei>(m_indices.size());
}