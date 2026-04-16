#include "Scene.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Skybox.hpp"

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
    // ===== SHADERS (ENTEROS) ==
    // ==========================
    const string Scene::vertex_shader_code =
        "#version 330 core\n"
        "uniform mat4 model_view_matrix;\n"
        "uniform mat4 projection_matrix;\n"
        "layout(location = 0) in vec3 vertex_coordinates;\n"
        "layout(location = 1) in vec3 vertex_normal;\n"
        "out vec3 frag_color;\n"
        "out vec3 normal;\n"
        "out vec3 frag_pos;\n"
        "void main() {\n"
        "    vec4 world_pos = model_view_matrix * vec4(vertex_coordinates,1.0);\n"
        "    gl_Position = projection_matrix * world_pos;\n"
        "    normal = mat3(transpose(inverse(model_view_matrix))) * vertex_normal;\n"
        "    frag_pos = vec3(world_pos);\n"
        "    vec3 n = normalize(vertex_normal);\n"
        "    n = n * 0.5 + 0.3;\n"
        "    frag_color = n;\n"
        "}";

    const string Scene::fragment_shader_code =
        "#version 330 core\n"
        "in vec3 frag_color;\n"
        "in vec3 normal;\n"
        "in vec3 frag_pos;\n"
        "out vec4 fragment_color;\n"
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
        "    fragment_color = vec4(result, 0.4);\n"
        "}";

    // ==========================
    // ===== HELPERS ============
    // ==========================
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

    // ==========================
    // ===== CONSTRUCTOR ========
    // ==========================
    Scene::Scene(unsigned width, unsigned height)
        : angle(0.0f)
    {

        skybox = std::make_shared<Skybox>("../../shared/assets/sky-cube-map-");


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

        resize(width, height);
    }

    // ==========================
    // ===== UPDATE ============
    // ==========================
    void Scene::update()
    {
        angle += 0.01f;
    }

    // ==========================
    // ===== RENDER ============
    // ==========================
    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ============================
        // 1. DIBUJAR SKYBOX PRIMERO
        // ============================

        skybox->render(camera);     // <-- usa su propio shader

        // ============================
        // 2. ACTIVAR SHADER PRINCIPAL
        // ============================
        glUseProgram(program_id);

        // ============================
        // 3. MATRIZ DE VISTA
        // ============================
        glm::mat4 I(1.0f);
        glm::mat4 view = camera.get_view_matrix();

        // ============================================================
        // ----------------------   T E R R E N O   --------------------
        // ============================================================
        glm::mat4 model = glm::translate(I, glm::vec3(-100.f, -5.f, -200.f));
        glm::mat4 model_view = view * model;

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view));
        terrain.Draw();


        // ============================================================
        // -------------------   C U B O   G R A N D E   --------------
        // ============================================================
        glm::mat4 big_cube_model = make_model(
            I,
            glm::vec3(0.f, 0.f, -6.f),
            angle,
            glm::vec3(0.f, 1.f, 0.f),
            glm::vec3(1.2f)
        );

        glm::mat4 big_cube_mv = view * big_cube_model;

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(big_cube_mv));

        glDisable(GL_BLEND);

        cube.set_color(glm::vec3(
            0.5f + 0.5f * sin(angle),
            0.5f + 0.5f * cos(angle),
            0.5f + 0.5f * sin(angle * 0.5f)
        ));

        cube.render();


        // ============================================================
        // -------------------   C U B O   P E Q U E Ñ O   ------------
        // ============================================================
        glm::mat4 small_cube_model = I;
        small_cube_model = glm::translate(small_cube_model, glm::vec3(0.f, 0.f, -6.f));
        small_cube_model = glm::rotate(small_cube_model, angle * 2.f, glm::vec3(0, 1, 0));
        small_cube_model = glm::translate(small_cube_model, glm::vec3(4.f, 0.f, 0.f));
        small_cube_model = glm::rotate(small_cube_model, angle * 3.f, glm::vec3(1, 1, 0));
        small_cube_model = glm::scale(small_cube_model, glm::vec3(0.35f));

        glm::mat4 small_cube_mv = view * small_cube_model;

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(small_cube_mv));

        glEnable(GL_BLEND);

        cube.set_color(glm::vec4(
            0.5f + 0.5f * cos(angle * 1.5f),
            0.5f + 0.5f * sin(angle * 0.7f),
            0.5f + 0.5f * cos(angle * 2.0f),
            0.5f
        ));

        cube.render();

        glDisable(GL_BLEND);
    }


    // ==========================
    // ===== CONTROLES ==========
    // ==========================
    void Scene::moveForward(float dt) { camera.moveForward(dt); }
    void Scene::moveBackward(float dt) { camera.moveBackward(dt); }
    void Scene::moveLeft(float dt) { camera.moveLeft(dt); }
    void Scene::moveRight(float dt) { camera.moveRight(dt); }
    void Scene::moveUp(float dt) { camera.moveUp(dt); }
    void Scene::moveDown(float dt) { camera.moveDown(dt); }
    void Scene::rotateCamera(float dx, float dy) { camera.rotate(dx, dy); }

    // ==========================
    // ===== MOUSE ============
    // ==========================
    void Scene::handleMouse(float dx, float dy, float dt)
    {
        camera.rotate(dx, dy);

        const float speed = 5.0f * dt;

        float dirX, dirY, dirZ;
        camera.getDirection(dirX, dirY, dirZ);

        glm::vec3 forward = glm::normalize(glm::vec3(dirX, dirY, dirZ));
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

        if (std::fabs(dy) > 1.0f)
        {
            camera.setPosition(
                camera.getX() + forward.x * speed * (dy < 0 ? 1 : -1),
                camera.getY() + forward.y * speed * (dy < 0 ? 1 : -1),
                camera.getZ() + forward.z * speed * (dy < 0 ? 1 : -1)
            );
        }

        if (std::fabs(dx) > 1.0f)
        {
            camera.setPosition(
                camera.getX() + right.x * speed * (dx > 0 ? 1 : -1),
                camera.getY() + right.y * speed * (dx > 0 ? 1 : -1),
                camera.getZ() + right.z * speed * (dx > 0 ? 1 : -1)
            );
        }
    }

    // ==========================
    // ===== RESIZE ============
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