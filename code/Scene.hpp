#pragma once

#include "Cube.hpp"
#include "Model.hpp"
#include "Node.hpp"
#include "Skybox.hpp"
#include "Shader_Program.hpp"
#include "Terrain.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Texture2D.hpp"

#include <memory>
#include <string>

#include <glad/gl.h>

namespace udit
{
    class Scene
    {
    private:

        // ================= SHADERS =================
        static const std::string vertex_shader_code;
        static const std::string fragment_shader_code;

        // ================= GRAFO =================
        Node root;   // NO shared_ptr (evita errores de set_parent)

        std::shared_ptr<Model> terrain_node;
        std::shared_ptr<Model> earth_node;
        std::shared_ptr<Model> moon_node;

        GLuint program_id;

        GLint model_view_matrix_id;
        GLint projection_matrix_id;

        // ================= OBJETOS =================
        Cube cube;
        Terrain terrain;
        Camera camera;
        Light light;

        std::shared_ptr<Skybox> skybox;
        std::shared_ptr<Texture2D> texture_wood;

        // ================= TEXTURAS =================
        GLuint tex_circulo = 0;
        GLuint tex_earth = 0;
        GLuint tex_luna = 0;
        GLuint tex_sky6 = 0;
        GLuint tex_terrain = 0;

        // ================= ESTADO =================
        float angle;
        float mixValue = 0.5f;

        // ================= HELPERS =================
        GLuint compile_shaders();
        void show_compilation_error(GLuint shader_id);
        void show_linkage_error(GLuint program_id);

        void load_textures();

    public:

        Scene(unsigned width, unsigned height);

        void update();
        void render();
        void resize(unsigned width, unsigned height);

        void moveForward(float dt);
        void moveBackward(float dt);
        void moveLeft(float dt);
        void moveRight(float dt);
        void moveUp(float dt);
        void moveDown(float dt);

        void rotateCamera(float dx, float dy);
        void handleMouse(float dx, float dy, float dt);
    };
}