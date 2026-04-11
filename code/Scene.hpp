#pragma once

#include "Cube.hpp"
#include "Skybox.hpp"
#include "Shader_Program.hpp"
#include "Terrain.hpp"
#include "Camera.hpp"
#include "Light.hpp"

#include <memory>
#include <string>

#include <glad/gl.h>   //NECESARIO para GLuint / GLint

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

        Terrain terrain;
        Camera camera;
        Light light;

        std::shared_ptr<Skybox> skybox;
        GLuint skybox_shader;

        // ================= TEXTURA (AÑADIDO) =================
        GLuint texture_id;          // textura del cubo
        bool there_is_texture;      // control de uso

        static const std::string texture_path;

        GLuint create_texture_2d(const std::string& texture_path);
        std::unique_ptr<class Color_Buffer> load_image(const std::string& image_path);

        // ======================================================

        GLuint compile_shaders();
        void show_compilation_error(GLuint shader_id);
        void show_linkage_error(GLuint program_id);

    public:
        Scene(unsigned width, unsigned height);

        void update();
        void render();
        void resize(unsigned width, unsigned height);

        // ================= INPUT =================

        void moveForward(float dt);
        void moveBackward(float dt);
        void moveLeft(float dt);
        void moveRight(float dt);
        void moveUp(float dt);
        void moveDown(float dt);

        void rotateCamera(float dx, float dy);
        void handleMouse(float dx, float dy, float dt);

        GLuint program_id;
    };
}