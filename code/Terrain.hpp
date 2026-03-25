#pragma once

#include <vector>
#include <glm.hpp>
#include <glad/gl.h>

// ----- Estructura de un vertice -----
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& uv)
        : position(p), normal(n), texCoords(uv) {
    }
};

// ----- Clase Terrain -----
class Terrain
{
public:
    Terrain(int width = 200, int height = 200, float scale = 1.0f);
    ~Terrain();

    void Draw() const;

private:
    int m_width;
    int m_height;
    float m_scale;

    std::vector<float> m_heights;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    GLuint vao_id = 0;
    GLuint vbo_id = 0;
    GLuint ebo_id = 0;
    GLsizei index_count = 0;

    void GenerateHeightmap();
    void BuildMesh();
    void UploadToGPU();
    float GetHeight(int x, int z) const;
    glm::vec3 CalculateNormal(int x, int z) const;
};