#include "Scene.hpp"

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
    "uniform mat4 model_view_matrix;"
    "uniform mat4 projection_matrix;"
    "layout(location = 0) in vec3 vertex_coordinates;"
    "layout(location = 1) in vec3 vertex_normal;"
    "out vec3 frag_color;"
    "void main() {"
    "   gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates,1.0);"
    "   vec3 n = normalize(vertex_normal);"
    "   n = n * 0.5 + 0.3;" // aumenta contraste y brillo metálico
    "   frag_color = n;"
    "}";

const string Scene::fragment_shader_code =
    "#version 330 core\n"
    "in vec3 frag_color;"
    "out vec4 fragment_color;"
    "void main() {"
    "   vec3 color = pow(frag_color, vec3(1.5));" // acentúa los brillos
    "   fragment_color = vec4(color, 1.0);"
    "}";

    // ================= CONSTRUCTOR =================
    Scene::Scene(unsigned width, unsigned height)
        : angle(0), terrain(200, 200, 1.0f)  // Inicializamos el terreno
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // Color de fondo
        glm::vec3 top = glm::vec3(0.65f, 0.90f, 1.0f);
        glm::vec3 bottom = glm::vec3(0.20f, 0.45f, 0.75f);
        float t = 0.09f;
        glm::vec3 color = glm::mix(bottom, top, t);
        glClearColor(color.r, color.g, color.b, 1.0f);

        // Compila los shaders
        program_id = compile_shaders();
        glUseProgram(program_id);

        // Obtiene las ubicaciones de las matrices
        model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");

        // Ajusta la ventana
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

        // --- Terreno ---
        glm::mat4 terrain_matrix = glm::mat4(1.0f);
        terrain_matrix = glm::translate(terrain_matrix, glm::vec3(-100.f, -5.f, -200.f)); // mover al centro de la escena
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(terrain_matrix));
        terrain.Draw();

        // --- Cubo grande ---
        glm::mat4 model_view_matrix = glm::mat4(1.0f);
        model_view_matrix = glm::translate(model_view_matrix, glm::vec3(0.f, 0.f, -6.f));
        model_view_matrix = glm::rotate(model_view_matrix, angle, glm::vec3(0.f, 1.f, 0.f));
        // Escala reducida del cubo grande
        model_view_matrix = glm::scale(model_view_matrix, glm::vec3(1.2f));
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

        glm::vec3 dynamic_color_big(
            0.5f + 0.5f * sin(angle),
            0.5f + 0.5f * cos(angle),
            0.5f + 0.5f * sin(angle * 0.5f)
        );
        cube.set_color(dynamic_color_big);
        cube.render();

        // ==================== CUBO PEQUEÑO ORBITANDO ====================
        glm::mat4 small_cube = glm::mat4(1.0f);
        small_cube = glm::translate(small_cube, glm::vec3(0.f, 0.f, -6.f));
        small_cube = glm::rotate(small_cube, angle * 2.0f, glm::vec3(0.f, 1.f, 0.f));
        small_cube = glm::translate(small_cube, glm::vec3(4.f, 0.f, 0.f)); // distancia desde el cubo grande
        small_cube = glm::rotate(small_cube, angle * 3.0f, glm::vec3(1.f, 1.f, 0.f));
        // Escala reducida del cubo pequeño
        small_cube = glm::scale(small_cube, glm::vec3(0.35f));
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(small_cube));

        glm::vec3 dynamic_color_small(
            0.5f + 0.5f * cos(angle * 1.5f),
            0.5f + 0.5f * sin(angle * 0.7f),
            0.5f + 0.5f * cos(angle * 2.0f)
        );
        cube.set_color(dynamic_color_small);
        cube.render();

    }

    // ================= RESIZE =================
    void Scene::resize(unsigned width, unsigned height)
    {
        glm::mat4 projection_matrix = glm::perspective(glm::radians(45.f), GLfloat(width) / height, 1.f, 5000.f);
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