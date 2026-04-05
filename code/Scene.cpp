#include "Scene.hpp"
#include "Camera.hpp"
#include "Light.hpp"

#include <iostream>
#include <cassert>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace udit
{
    using namespace std;

    // ================= SHADERS =================
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

        // Ambient un poco más fuerte
        "    float ambient_strength = 0.2;\n"
        "    vec3 ambient = ambient_strength * frag_color;\n"

        // Diffuse
        "    float diff = max(dot(norm, light_dir), 0.0);\n"
        "    vec3 diffuse = diff * frag_color;\n"

        // Specular un poco más brillante
        "    float specular_strength = 0.7;\n"
        "    vec3 view_dir = normalize(view_pos - frag_pos);\n"
        "    vec3 reflect_dir = reflect(-light_dir, norm);\n"
        "    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);\n"
        "    vec3 specular = specular_strength * spec * vec3(1.0);\n"

        // Resultado final
        "    vec3 result = ambient + diffuse + specular;\n"

        // Suavizar el pow final para no oscurecer
        "    result = pow(result, vec3(1.3));\n"

        "    fragment_color = vec4(result, 0.4);\n"
        "}";

    // ================= CONSTRUCTOR =================
    Scene::Scene(unsigned width, unsigned height)
        : angle(0), terrain(200, 200, 1.0f) // Inicializamos el terreno
    {
        // Configuración básica de OpenGL
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Color de fondo
        glm::vec3 top(0.65f, 0.90f, 1.0f);
        glm::vec3 bottom(0.20f, 0.45f, 0.75f);
        float t = 0.09f;
        glm::vec3 bg_color = glm::mix(bottom, top, t);
        glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);

        // Compilación de shaders
        program_id = compile_shaders();
        glUseProgram(program_id);

        // Obtención de ubicaciones de matrices
        model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");

        // Ajuste inicial de la ventana
        resize(width, height);
    }

    // ================= UPDATE =================
    void Scene::update()
    {
        angle += 0.01f;
    }

    // ================= RENDER =================
    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program_id);

        // ---------------- TERRAIN ----------------
        glm::mat4 terrain_matrix = glm::mat4(1.0f);
        terrain_matrix = glm::translate(terrain_matrix, glm::vec3(-100.f, -5.f, -200.f));
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(terrain_matrix));
        terrain.Draw();

        // ---------------- CUBO GRANDE ----------------
        glm::mat4 model_view_matrix = glm::mat4(1.0f);
        model_view_matrix = glm::translate(model_view_matrix, glm::vec3(0.f, 0.f, -6.f));
        model_view_matrix = glm::rotate(model_view_matrix, angle, glm::vec3(0.f, 1.f, 0.f));
        model_view_matrix = glm::scale(model_view_matrix, glm::vec3(1.2f));

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

        // Desactivar blending para el cubo grande
        glDisable(GL_BLEND);

        glm::vec3 dynamic_color_big(
            0.5f + 0.5f * sin(angle),
            0.5f + 0.5f * cos(angle),
            0.5f + 0.5f * sin(angle * 0.5f)
        );
        cube.set_color(dynamic_color_big); // Mantiene su degradado metálico
        cube.render();

        // ---------------- CUBO PEQUEÑO ORBITANDO ----------------
        glm::mat4 small_cube = glm::mat4(1.0f);
        small_cube = glm::translate(small_cube, glm::vec3(0.f, 0.f, -6.f));
        small_cube = glm::rotate(small_cube, angle * 2.0f, glm::vec3(0.f, 1.f, 0.f));
        small_cube = glm::translate(small_cube, glm::vec3(4.f, 0.f, 0.f));
        small_cube = glm::rotate(small_cube, angle * 3.0f, glm::vec3(1.f, 1.f, 0.f));
        small_cube = glm::scale(small_cube, glm::vec3(0.35f));

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(small_cube));

        // Activar blending solo para el cubo pequeño
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Mezcla estándar para transparencia

        glm::vec4 dynamic_color_small(
            0.5f + 0.5f * cos(angle * 1.5f),
            0.5f + 0.5f * sin(angle * 0.7f),
            0.5f + 0.5f * cos(angle * 2.0f),
            0.5f // Alpha < 1 para que sea transparente
        );
        cube.set_color(dynamic_color_small);
        cube.render();

        // Después de renderizar el cubo pequeño, si vas a dibujar otros objetos opacos, recuerda:
        glDisable(GL_BLEND);

        // ---------------- CAMERA VIEW ----------------
        glm::vec3 cameraPos(camera.getX(), camera.getY(), camera.getZ());

        float dirX, dirY, dirZ;
        camera.getDirection(dirX, dirY, dirZ);

        glm::vec3 cameraFront = glm::normalize(glm::vec3(dirX, dirY, dirZ));
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    // ================= CONTROLES =================

 // Puedes dejar estos por si quieres combinar teclado + ratón
    void Scene::moveForward(float dt) { camera.moveForward(dt); }
    void Scene::moveBackward(float dt) { camera.moveBackward(dt); }
    void Scene::moveLeft(float dt) { camera.moveLeft(dt); }
    void Scene::moveRight(float dt) { camera.moveRight(dt); }
    void Scene::moveUp(float dt) { camera.moveUp(dt); }
    void Scene::moveDown(float dt) { camera.moveDown(dt); }

    // Rotación directa (opcional)
    void Scene::rotateCamera(float dx, float dy) {
        camera.rotate(dx, dy);
    }

    // ================= RATÓN TOTAL (PRO) =================
    void Scene::handleMouse(float dx, float dy, float dt)
    {
        // 1. ROTACIÓN SIEMPRE ACTIVA (mirar libre)
        camera.rotate(dx, dy);

        // 2. MOVIMIENTO SUAVE PROPORCIONAL
        float movementForce = 0.01f;   // controla la velocidad general
        float deadZone = 1.0f;         // evita micro-movimientos molestos

        // ===== ADELANTE / ATRÁS =====
        if (fabs(dy) > deadZone)
        {
            float amount = fabs(dy) * movementForce;

            if (dy < 0)
                camera.moveForward(dt * amount);
            else
                camera.moveBackward(dt * amount);
        }

        // ===== STRAFE IZQ / DER =====
        if (fabs(dx) > deadZone)
        {
            float amount = fabs(dx) * movementForce;

            if (dx > 0)
                camera.moveRight(dt * amount);
            else
                camera.moveLeft(dt * amount);
        }
    }


    // ================= RESIZE =================
    void Scene::resize(unsigned width, unsigned height)
    {
        glm::mat4 projection_matrix = glm::perspective(glm::radians(45.f),
            GLfloat(width) / height,
            1.f, 5000.f);
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        glViewport(0, 0, width, height);
    }

    // ================= SHADER HELPERS =================
    GLuint Scene::compile_shaders()
    {
        GLint success = GL_FALSE;

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        const char* v_code = vertex_shader_code.c_str();
        const char* f_code = fragment_shader_code.c_str();

        glShaderSource(vertex_shader, 1, &v_code, nullptr);
        glShaderSource(fragment_shader, 1, &f_code, nullptr);

        glCompileShader(vertex_shader);
        glCompileShader(fragment_shader);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) show_compilation_error(vertex_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) show_compilation_error(fragment_shader);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vertex_shader);
        glAttachShader(prog, fragment_shader);
        glLinkProgram(prog);

        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) show_linkage_error(prog);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return prog;
    }

    void Scene::show_compilation_error(GLuint shader_id)
    {
        GLint logLength;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(logLength, ' ');
        glGetShaderInfoLog(shader_id, logLength, nullptr, &log[0]);
        std::cerr << log << std::endl;
        assert(false);
    }

    void Scene::show_linkage_error(GLuint program_id)
    {
        GLint logLength;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(logLength, ' ');
        glGetProgramInfoLog(program_id, logLength, nullptr, &log[0]);
        std::cerr << log << std::endl;
        assert(false);
    }
}