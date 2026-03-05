#pragma once

#include <glm.hpp>
#include <vector>
#include <GL/glew.h>

namespace udit
{
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
    };

    class Terrain {
    public:
        Terrain(int width, int depth);
        ~Terrain();

        void generate(int width, int depth);
        void render(const glm::mat4& proj, const glm::mat4& view, GLuint shader_prog, const glm::vec3& light_pos, const glm::vec3& view_pos);

        static float height(float x, float z);

        GLuint vao() const { return vao_; }
        GLsizei count() const { return count_; }

    private:
        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLsizei count_ = 0;

        int width_;
        int depth_;

        void create_buffers(const std::vector<Vertex>& vertices);
        glm::vec3 compute_normal(int x, int z) const;
        float h(int i, int j) const;
    };
}
