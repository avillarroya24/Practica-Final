#pragma once

#include "Cube.hpp"
#include "Skybox.hpp"
#include "Shader_Program.hpp"

#include <memory>
#include <string>

namespace udit
{
    class Scene
    {
    private:
        static const std::string vertex_shader_code;
        static const std::string fragment_shader_code;

        GLint model_view_matrix_id;
        GLint projection_matrix_id;

        Cube cube;
        float angle;

        std::shared_ptr<Skybox> skybox;

        GLuint compile_shaders();
        void show_compilation_error(GLuint shader_id);
        void show_linkage_error(GLuint program_id);

    public:
        Scene(unsigned width, unsigned height);
        void update();
        void render();
        void resize(unsigned width, unsigned height);

        GLuint program_id;
    };
}