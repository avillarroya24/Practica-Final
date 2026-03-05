#include "terrain.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace udit
{
    Terrain::Terrain(int width, int depth)
        : width_(width), depth_(depth)
    {
        generate(width, depth);
    }

    Terrain::~Terrain()
    {
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (vao_) glDeleteVertexArrays(1, &vao_);
    }

    void Terrain::generate(int width, int depth)
    {
        width_ = width;
        depth_ = depth;

        std::vector<Vertex> vertices;
        float sizeX = 1.0f;
        float sizeZ = 1.0f;

        for (int z = 0; z < depth - 1; ++z) {
            for (int x = 0; x < width - 1; ++x) {
                glm::vec3 v0(x * sizeX, h(x, z), z * sizeZ);
                glm::vec3 v1((x + 1) * sizeX, h(x + 1, z), z * sizeZ);
                glm::vec3 v2(x * sizeX, h(x, z + 1), (z + 1) * sizeZ);
                glm::vec3 v3((x + 1) * sizeX, h(x + 1, z + 1), (z + 1) * sizeZ);

                glm::vec3 n0 = compute_normal(x, z);
                glm::vec3 n1 = compute_normal(x + 1, z);
                glm::vec3 n2 = compute_normal(x, z + 1);
                glm::vec3 n3 = compute_normal(x + 1, z + 1);

                vertices.push_back({ v0, n0 });
                vertices.push_back({ v2, n2 });
                vertices.push_back({ v3, n3 });
                vertices.push_back({ v0, n0 });
                vertices.push_back({ v3, n3 });
                vertices.push_back({ v1, n1 });
            }
        }

        create_buffers(vertices);
    }

    void Terrain::create_buffers(const std::vector<Vertex>& vertices)
    {
        count_ = static_cast<GLsizei>(vertices.size());

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glBindVertexArray(0);
    }

    glm::vec3 Terrain::compute_normal(int x, int z) const
    {
        float hL = h(x - 1, z);
        float hR = h(x + 1, z);
        float hD = h(x, z - 1);
        float hU = h(x, z + 1);
        glm::vec3 n(-(hR - hL), 2.0f, -(hU - hD));
        return glm::normalize(n);
    }

    float Terrain::h(int i, int j) const
    {
        float base = 0.5f * sin(i * 0.25f) * cos(j * 0.25f);
        float detail = 0.25f * sin(j * 0.5f);
        return base + detail;
    }

    float Terrain::height(float x, float z)
    {
        float base = 0.5f * sin(x * 0.25f) * cos(z * 0.25f);
        float detail = 0.25f * sin(z * 0.5f);
        return base + detail;
    }

    void Terrain::render(const glm::mat4& proj, const glm::mat4& view, GLuint shader_prog, const glm::vec3& light_pos, const glm::vec3& view_pos)
    {
        glUseProgram(shader_prog);
        GLint loc_mvp = glGetUniformLocation(shader_prog, "MVP");
        GLint loc_model = glGetUniformLocation(shader_prog, "model");
        GLint loc_color = glGetUniformLocation(shader_prog, "u_color");
        GLint loc_light = glGetUniformLocation(shader_prog, "u_light_pos");
        GLint loc_view = glGetUniformLocation(shader_prog, "u_view_pos");
        GLint loc_alpha = glGetUniformLocation(shader_prog, "u_alpha");

        glBindVertexArray(vao_);

        const int tile_range = 2;
        float tile_size = 30.0f;
        for (int dz = -tile_range; dz <= tile_range; ++dz)
            for (int dx = -tile_range; dx <= tile_range; ++dx)
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(dx * tile_size, 0.f, dz * tile_size));

                glm::mat4 mvp = proj * view * model;

                glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3f(loc_color, 0.9f, 0.85f, 0.7f);
                glUniform3f(loc_light, light_pos.x, light_pos.y, light_pos.z);
                glUniform3f(loc_view, view_pos.x, view_pos.y, view_pos.z);
                glUniform1f(loc_alpha, 1.0f);

                glDrawArrays(GL_TRIANGLES, 0, count_);
            }

        glBindVertexArray(0);
    }
}
