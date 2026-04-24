#include "Scene.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Skybox.hpp"
#include "Texture2D.hpp"

#include <iostream>
#include <cassert>
#include <cmath>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace udit
{
    using namespace std;

    // ==========================
    // ===== SHADERS ============
    // ==========================
    const string Scene::vertex_shader_code =
        "#version 330 core\n"
        "uniform mat4 model_view_matrix;\n"
        "uniform mat4 projection_matrix;\n"
        "layout(location = 0) in vec3 vertex_coordinates;\n"
        "layout(location = 1) in vec3 vertex_normal;\n"
        "layout(location = 2) in vec2 vertex_uv;\n"
        "out vec3 frag_color;\n"
        "out vec3 normal;\n"
        "out vec3 frag_pos;\n"
        "out vec2 texcoord;\n"
        "void main() {\n"
        "    vec4 world_pos = model_view_matrix * vec4(vertex_coordinates,1.0);\n"
        "    gl_Position = projection_matrix * world_pos;\n"
        "    normal = mat3(transpose(inverse(model_view_matrix))) * vertex_normal;\n"
        "    frag_pos = vec3(world_pos);\n"
        "    texcoord = vertex_uv;\n"
        "    vec3 n = normalize(vertex_normal);\n"
        "    n = n * 0.5 + 0.3;\n"
        "    frag_color = n;\n"
        "}";

    const string Scene::fragment_shader_code =
        "#version 330 core\n"
        "in vec3 frag_color;\n"
        "in vec3 normal;\n"
        "in vec3 frag_pos;\n"
        "in vec2 texcoord;\n"
        "out vec4 fragment_color;\n"
        "uniform sampler2D diffuse_map;\n"
        "uniform bool use_texture;\n"
        "uniform float uv_scale;\n" // AÑADIDO
        "uniform vec3 light_pos;\n"
        "uniform vec3 view_pos;\n"
        "void main() {\n"
        "    vec3 norm = normalize(normal);\n"
        "    vec3 light_dir = normalize(light_pos - frag_pos);\n"
        "    float ambient_strength = 0.2;\n"
        "    vec3 ambient = ambient_strength * frag_color;\n"
        "    float diff = max(dot(norm, light_dir), 0.0);\n"
        "    vec3 diffuse = diff * frag_color;\n"
        "    float specular_strength = 0.7;\n"
        "    vec3 view_dir = normalize(view_pos - frag_pos);\n"
        "    vec3 reflect_dir = reflect(-light_dir, norm);\n"
        "    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);\n"
        "    vec3 specular = specular_strength * spec * vec3(1.0);\n"
        "    vec3 result = ambient + diffuse + specular;\n"
        "    result = pow(result, vec3(1.3));\n"
        "    if (use_texture)\n"
        "        fragment_color = vec4(result, 1.0) * texture(diffuse_map, texcoord * uv_scale);\n" // AÑADIDO
        "    else\n"
        "        fragment_color = vec4(result, 1.0);\n"
        "}";

    namespace
    {
        constexpr glm::vec3 TERRAIN_OFFSET = glm::vec3(-100.f, -5.f, -200.f);
        constexpr glm::vec3 MAIN_CUBE_POS = glm::vec3(0.f, 0.f, -6.f);
        constexpr float MAIN_CUBE_SCALE = 1.2f;

        constexpr glm::vec3 SMALL_CUBE_BASE_POS = glm::vec3(0.f, 0.f, -6.f);
        constexpr glm::vec3 SMALL_CUBE_OFFSET = glm::vec3(4.f, 0.f, 0.f);
        constexpr float SMALL_CUBE_SCALE = 0.35f;
    }

    static glm::mat4 make_model(
        const glm::mat4& base,
        const glm::vec3& t,
        float rot,
        const glm::vec3& axis,
        const glm::vec3& scale)
    {
        glm::mat4 m = base;
        m = glm::translate(m, t);
        m = glm::rotate(m, rot, axis);
        m = glm::scale(m, scale);
        return m;
    }

    Scene::Scene(unsigned width, unsigned height)
        : angle(0.0f)
    {
        skybox = std::make_shared<Skybox>("../../shared/assets/sky-cube-map-");

        Texture2D::load_default_textures();

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.2f, 0.4f, 0.9f, 1.0f);

        program_id = compile_shaders();
        glUseProgram(program_id);

        model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");

        glUniform1i(glGetUniformLocation(program_id, "diffuse_map"), 0);
        glUniform1f(glGetUniformLocation(program_id, "uv_scale"), 1.0f); // AÑADIDO

        resize(width, height);
    }

    void Scene::update()
    {
        angle += 0.01f;
    }

    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox->render(camera);

        glUseProgram(program_id);

        glm::mat4 I(1.0f);
        glm::mat4 view = camera.get_view_matrix();

        int useTexLoc = glGetUniformLocation(program_id, "use_texture");

        // =========================
        // ===== TERRAIN ===========
        // =========================
        auto terrainTex = Texture2D::get("terrain");

        if (terrainTex)
        {
            terrainTex->bind(0);
            glUniform1i(useTexLoc, 1);
            glUniform1f(glGetUniformLocation(program_id, "uv_scale"), 8.0f);
        }
        else
        {
            glUniform1i(useTexLoc, 0);
        }

        glm::mat4 terrain_model = glm::translate(I, TERRAIN_OFFSET);
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE,
            glm::value_ptr(view * terrain_model));

        terrain.Draw();

        // =========================
        // ===== MAIN CUBE =========
        // =========================
        auto earthTex = Texture2D::get("earth");

        if (earthTex)
        {
            cube.set_texture(earthTex->get_id());   // asigna textura al cubo
            cube.enable_texture(true);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, earthTex->get_id());

            glUniform1i(useTexLoc, 1);
        }
        else
        {
            cube.enable_texture(false);
            glUniform1i(useTexLoc, 0);
        }

        glm::mat4 main_cube_model = make_model(
            I,
            MAIN_CUBE_POS,
            angle * 0.5f,   // más lento = más realista
            glm::vec3(0.f, 1.f, 0.f),
            glm::vec3(MAIN_CUBE_SCALE)
        );

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE,
            glm::value_ptr(view * main_cube_model));

        cube.render();

        // =========================
        // ===== SMALL CUBE ========
        // =========================
        glUniform1i(useTexLoc, 0); // sin textura explícito

        glm::mat4 small_cube_model = I;
        small_cube_model = glm::translate(small_cube_model, SMALL_CUBE_BASE_POS);
        small_cube_model = glm::rotate(small_cube_model, angle * 2.f, glm::vec3(0, 1, 0));
        small_cube_model = glm::translate(small_cube_model, SMALL_CUBE_OFFSET);
        small_cube_model = glm::rotate(small_cube_model, angle * 3.f, glm::vec3(1, 1, 0));
        small_cube_model = glm::scale(small_cube_model, glm::vec3(SMALL_CUBE_SCALE));

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE,
            glm::value_ptr(view * small_cube_model));

        cube.enable_texture(false);

        cube.set_color(glm::vec4(
            0.5f + 0.5f * cos(angle * 1.5f),
            0.5f + 0.5f * sin(angle * 0.7f),
            0.5f + 0.5f * cos(angle * 2.0f),
            0.5f
        ));

        small_cube_model = glm::rotate(
            small_cube_model,
            angle * 0.8f,
            glm::vec3(0, 1, 0)
        );

        cube.render();

        glDisable(GL_BLEND);
    }

    // ==========================
    // ===== CONTROLS ===========
    // ==========================
    void Scene::moveForward(float dt) { camera.moveForward(dt); }
    void Scene::moveBackward(float dt) { camera.moveBackward(dt); }
    void Scene::moveLeft(float dt) { camera.moveLeft(dt); }
    void Scene::moveRight(float dt) { camera.moveRight(dt); }
    void Scene::moveUp(float dt) { camera.moveUp(dt); }
    void Scene::moveDown(float dt) { camera.moveDown(dt); }
    void Scene::rotateCamera(float dx, float dy) { camera.rotate(dx, dy); }

    void Scene::handleMouse(float dx, float dy, float dt)
    {
        camera.rotate(dx, dy);

        const float speed = 5.0f * dt;

        float dirX, dirY, dirZ;
        camera.getDirection(dirX, dirY, dirZ);

        glm::vec3 forward = glm::normalize(glm::vec3(dirX, dirY, dirZ));
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

        if (fabs(dy) > 1.0f)
        {
            camera.setPosition(
                camera.getX() + forward.x * speed * (dy < 0 ? 1 : -1),
                camera.getY() + forward.y * speed * (dy < 0 ? 1 : -1),
                camera.getZ() + forward.z * speed * (dy < 0 ? 1 : -1)
            );
        }

        if (fabs(dx) > 1.0f)
        {
            camera.setPosition(
                camera.getX() + right.x * speed * (dx > 0 ? 1 : -1),
                camera.getY() + right.y * speed * (dx > 0 ? 1 : -1),
                camera.getZ() + right.z * speed * (dx > 0 ? 1 : -1)
            );
        }
    }

    // ==========================
    // ===== RESIZE =============
    // ==========================
    void Scene::resize(unsigned width, unsigned height)
    {
        camera.setRatio(float(width) / height);

        glm::mat4 proj = glm::perspective(
            glm::radians(45.f),
            float(width) / height,
            1.f,
            5000.f
        );

        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(proj));
        glViewport(0, 0, width, height);
    }

    // ==========================
    // ===== SHADERS ============
    // ==========================
    GLuint Scene::compile_shaders()
    {
        GLint success;

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        const char* v = vertex_shader_code.c_str();
        const char* f = fragment_shader_code.c_str();

        glShaderSource(vs, 1, &v, nullptr);
        glShaderSource(fs, 1, &f, nullptr);

        glCompileShader(vs);
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) show_compilation_error(vs);

        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success) show_compilation_error(fs);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);

        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) show_linkage_error(prog);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return prog;
    }

    void Scene::show_compilation_error(GLuint id)
    {
        GLint len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

        std::string log(len, '\0');
        glGetShaderInfoLog(id, len, nullptr, log.data());

        std::cerr << log << std::endl;
        assert(false);
    }

    void Scene::show_linkage_error(GLuint id)
    {
        GLint len;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);

        std::string log(len, '\0');
        glGetProgramInfoLog(id, len, nullptr, log.data());

        std::cerr << log << std::endl;
        assert(false);
    }

}