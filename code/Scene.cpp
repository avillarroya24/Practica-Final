#include "Scene.hpp"

#include <iostream>
#include <cassert>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace udit
{
    using namespace std;

    // ================= SHADERS NORMALES =================
    const string Scene::vertex_shader_code =
        "#version 330\n"
        "uniform mat4 model_view_matrix;"
        "uniform mat4 projection_matrix;"
        "layout (location = 0) in vec3 vertex_coordinates;"
        "layout (location = 1) in vec3 vertex_color;"
        "out vec3 front_color;"
        "void main(){"
        "   gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);"
        "   front_color = vertex_color;"
        "}";

    const string Scene::fragment_shader_code =
        "#version 330\n"
        "in vec3 front_color;"
        "out vec4 fragment_color;"
        "void main(){"
        "    fragment_color = vec4(front_color, 1.0);"
        "}";

    // ================= CONSTRUCTOR =================
    Scene::Scene(unsigned width, unsigned height)
        : angle(0)
    {
        // Configuración de OpenGL
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.2f, 1.0f);

        // Compila y activa los shaders principales de la escena
        program_id = compile_shaders();
        glUseProgram(program_id);

        // Obtén las ubicaciones de las matrices de transformación
        model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");

        // Configura tamaño de ventana y proyección
        resize(width, height);

        // ===== SKYBOX =====
        std::shared_ptr<Shader_Program> skybox_shader =
            std::make_shared<Shader_Program>(
                Skybox::vertex_shader_code,
                Skybox::fragment_shader_code
            );

        // Inicializa el skybox con el shader y la ruta de texturas
        skybox = std::make_shared<Skybox>(
            "../../shared/assets/sky-cube-map-", // Ruta base de las texturas
            skybox_shader
        );
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

        // --- Cubo grande ---
        glm::mat4 model_view_matrix = glm::mat4(1.0f);
        model_view_matrix = glm::translate(model_view_matrix, glm::vec3(0.f, 0.f, -6.f));
        model_view_matrix = glm::rotate(model_view_matrix, angle, glm::vec3(0.f, 1.f, 0.f));
        model_view_matrix = glm::scale(model_view_matrix, glm::vec3(1.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

        // Generar color dinámico según ángulo
        glm::vec3 dynamic_color_big(
            0.5f + 0.5f * sin(angle),
            0.5f + 0.5f * cos(angle),
            0.5f + 0.5f * sin(angle * 0.5f)
        );
        cube.set_color(dynamic_color_big); // <- tu Cube.hpp debe permitir set_color(glm::vec3)
        cube.render();

        // --- Cubo pequeño orbitando ---
        glm::mat4 small_cube = glm::mat4(1.0f);
        small_cube = glm::translate(small_cube, glm::vec3(0.f, 0.f, -6.f));
        small_cube = glm::rotate(small_cube, angle * 2.0f, glm::vec3(0.f, 1.f, 0.f));
        small_cube = glm::translate(small_cube, glm::vec3(4.f, 0.f, 0.f));
        small_cube = glm::rotate(small_cube, angle * 3.0f, glm::vec3(1.f, 1.f, 0.f));
        small_cube = glm::scale(small_cube, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(small_cube));

        glm::vec3 dynamic_color_small(
            0.5f + 0.5f * cos(angle * 1.5f),
            0.5f + 0.5f * sin(angle * 0.7f),
            0.5f + 0.5f * cos(angle * 2.0f)
        );
        cube.set_color(dynamic_color_small);
        cube.render();

        glm::vec3 color_factor = glm::vec3(
            (sin(angle) + 1.0f) / 2.0f,
            (cos(angle * 1.5f) + 1.0f) / 2.0f,
            (sin(angle * 0.7f) + 1.0f) / 2.0f
        );
        cube.set_color(color_factor);

        // ===== SKYBOX =====
        glDepthFunc(GL_LEQUAL);

        glm::mat4 view = glm::mat4(glm::mat3(glm::mat4(1.0f))); // eliminar traslación
        glm::mat4 projection = glm::perspective(glm::radians(60.f), 1.0f, 0.1f, 100.f);
        skybox->render(glm::value_ptr(view), glm::value_ptr(projection));

        glDepthFunc(GL_LESS);
    }

    // ================= RESIZE =================
    void Scene::resize(unsigned width, unsigned height)
    {
        glm::mat4 projection_matrix = glm::perspective(20.f, GLfloat(width) / height, 1.f, 5000.f);
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        glViewport(0, 0, width, height);
    }

    // ================= COMPILAR SHADERS =================
    GLuint Scene::compile_shaders()
    {
        GLint succeeded = GL_FALSE;

        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vertex_shaders_code[] = { vertex_shader_code.c_str() };
        const char* fragment_shaders_code[] = { fragment_shader_code.c_str() };
        const GLint vertex_shaders_size[] = { (GLint)vertex_shader_code.size() };
        const GLint fragment_shaders_size[] = { (GLint)fragment_shader_code.size() };

        glShaderSource(vertex_shader_id, 1, vertex_shaders_code, vertex_shaders_size);
        glShaderSource(fragment_shader_id, 1, fragment_shaders_code, fragment_shaders_size);

        glCompileShader(vertex_shader_id);
        glCompileShader(fragment_shader_id);

        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(vertex_shader_id);

        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(fragment_shader_id);

        GLuint program_id = glCreateProgram();
        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);
        glLinkProgram(program_id);

        glGetProgramiv(program_id, GL_LINK_STATUS, &succeeded);
        if (!succeeded) show_linkage_error(program_id);

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);

        return program_id;
    }

    void Scene::show_compilation_error(GLuint shader_id)
    {
        string info_log;
        GLint info_log_length;

        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log.resize(info_log_length);
        glGetShaderInfoLog(shader_id, info_log_length, NULL, &info_log.front());
        cerr << info_log.c_str() << endl;
        assert(false);
    }

    void Scene::show_linkage_error(GLuint program_id)
    {
        string info_log;
        GLint info_log_length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log.resize(info_log_length);
        glGetProgramInfoLog(program_id, info_log_length, NULL, &info_log.front());
        cerr << info_log.c_str() << endl;
        assert(false);
    }
}