#include "Terrain.hpp"
#include <cmath>
#include <iostream>
#include <random>

/*

   Implementación de la clase Terrain.
 
 * Este archivo genera un terreno procedimental basado en heightmap,
   construye su malla (vertices e índices) y la sube a la GPU para su renderizado.

*/

// ================= CONSTRUCTOR =================

/*
     Inicializa el terreno generando:
 * - Heightmap (alturas procedurales)
 * - Malla (vértices e índices)
 * - Buffers en GPU

*/

Terrain::Terrain(int width, int height, float scale)
    : m_width(width), m_height(height), m_scale(scale)
{
    GenerateHeightmap();
    BuildMesh();
    UploadToGPU();
}

// ================= DESTRUCTOR =================

/*
     Libera los recursos de OpenGL asociados:
 * - VAO
 * - VBO
 * - EBO

*/
Terrain::~Terrain()
{
    if (vao_id) glDeleteVertexArrays(1, &vao_id);
    if (vbo_id) glDeleteBuffers(1, &vbo_id);
    if (ebo_id) glDeleteBuffers(1, &ebo_id);
}

// ================= RENDER =================

//Dibuja el terreno

void Terrain::Draw() const
{
    if (!vao_id) return;
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


// ================= HEIGHTMAP =================

/*

   Genera el heightmap del terreno.

 * Utiliza funciones seno/coseno combinadas con ruido aleatorio
   para simular un terreno natural con variaciones suaves.

*/
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

// ================= ACCESO A ALTURA =================

//Obtiene la altura en una coordenada del terreno

float Terrain::GetHeight(int x, int z) const
{
    return m_heights[z * m_width + x];
}
// ================= NORMALES =================

/*

   Calcula la normal en un punto del terreno.
 
 * Se basa en diferencias de altura con los vecinos
   para aproximar la dirección de la superficie.

*/

glm::vec3 Terrain::CalculateNormal(int x, int z) const
{
    float hL = (x > 0) ? GetHeight(x - 1, z) : GetHeight(x, z);
    float hR = (x < m_width - 1) ? GetHeight(x + 1, z) : GetHeight(x, z);
    float hD = (z > 0) ? GetHeight(x, z - 1) : GetHeight(x, z);
    float hU = (z < m_height - 1) ? GetHeight(x, z + 1) : GetHeight(x, z);

    glm::vec3 normal(hL - hR, 2.0f, hD - hU);
    return glm::normalize(normal);
}


// ================= CONSTRUCCIÓN DE MALLA =================

/*
   Construye la malla del terreno.
 
   Genera:
 * - Vértices (posición, normal, UV)
 * - Índices para triángulos
    

*/

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

// ================= GPU UPLOAD =================

/*

   Sube la malla del terreno a la GPU.
 
 * Crea VAO, VBO y EBO y define los atributos:
 * - posición
 * - normal
 * - coordenadas UV

*/
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

    // ================= ATRIBUTOS =================

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