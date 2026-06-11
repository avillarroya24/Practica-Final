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
    // =========================================================================
    // CÓDIGO DE SHADERS (GLSL)
    // =========================================================================

    /**
     * Vertex Shader principal del motor gráfico.
     * Procesa la geometría multiplicando las posiciones por las matrices de transformación,
     * calcula las normales en el espacio de la cámara y traslada las coordenadas UV escaladas.
     */
    const std::string Scene::vertex_shader_code =
        "#version 330 core\n"
        "uniform mat4 model_view_matrix;\n"
        "uniform mat4 projection_matrix;\n"
        "uniform float uv_scale;\n"
        "layout(location = 0) in vec3 vertex_coordinates;\n"
        "layout(location = 1) in vec3 vertex_normal;\n"
        "layout(location = 2) in vec2 vertex_uv;\n"
        "out vec3 frag_color;\n"
        "out vec3 normal;\n"
        "out vec3 frag_pos;\n"
        "out vec2 texcoord;\n"
        "void main() {\n"
        "    vec4 world_pos = model_view_matrix * vec4(vertex_coordinates, 1.0);\n"
        "    gl_Position = projection_matrix * world_pos;\n"
        "    normal = normalize(mat3(transpose(inverse(model_view_matrix))) * vertex_normal);\n"
        "    frag_pos = vec3(world_pos);\n"
        "    texcoord = vertex_uv * uv_scale;\n"
        "    vec3 n = normalize(vertex_normal);\n"
        "    frag_color = n * 0.5 + 0.3;\n"
        "}";

    /**
     * Fragment Shader principal del motor gráfico.
     * Modificado para permitir el tintado verde del terreno sobre su textura difusa.
     */
    const std::string Scene::fragment_shader_code =
        "#version 330 core\n"
        "in vec3 frag_color;\n"
        "in vec3 normal;\n"
        "in vec3 frag_pos;\n"
        "in vec2 texcoord;\n"
        "out vec4 fragment_color;\n"
        "uniform sampler2D diffuse_map;\n"
        "uniform bool use_texture;\n"
        "uniform bool is_terrain;\n" // Uniform añadido para detectar el dibujo del terreno
        "uniform vec3 light_pos;\n"
        "uniform vec3 view_pos;\n"
        "void main() {\n"
        "    vec3 N = normalize(normal);\n"
        "    vec3 L = normalize(light_pos - frag_pos);\n"
        "    vec3 V = normalize(view_pos - frag_pos);\n"
        "    vec3 R = reflect(-L, N);\n"
        "    vec3 ambient = vec3(0.25);\n"
        "    float diff = max(dot(N, L), 0.0);\n"
        "    vec3 diffuse = diff * vec3(1.0);\n"
        "    float spec = pow(max(dot(V, R), 0.0), 16.0);\n"
        "    vec3 specular = spec * vec3(0.4);\n"
        "    vec3 light = ambient + diffuse + specular;\n"
        "    vec3 base = use_texture ? texture(diffuse_map, texcoord).rgb : frag_color;\n"
        "\n"
        "    // Si es el terreno, multiplicamos la textura por un tinte verde (RGB: 0.2, 0.8, 0.2)\n"
        "    if (is_terrain && use_texture) {\n"
        "        base *= vec3(0.2, 0.8, 0.2);\n"
        "    }\n"
        "\n"
        "    vec3 final_color = base * light;\n"
        "    final_color = pow(final_color, vec3(1.0 / 2.2));\n"
        "    fragment_color = vec4(final_color, 1.0);\n"
        "}";

    // =========================================================================
    // CONSTANTES DE CONFIGURACIÓN
    // =========================================================================

    namespace
    {
        constexpr glm::vec3 TERRAIN_OFFSET(-100.f, -5.f, -200.f);
        constexpr glm::vec3 MAIN_CUBE_POS(0.f, 0.f, -6.f);
        constexpr float MAIN_CUBE_SCALE = 1.2f;
        constexpr glm::vec3 SMALL_CUBE_BASE_POS(0.f, 0.f, -6.f);
        constexpr glm::vec3 SMALL_CUBE_OFFSET(4.f, 0.f, 0.f);
        constexpr float SMALL_CUBE_SCALE = 0.35f;
    }

    // =========================================================================
    // FUNCIONES DE UTILIDAD
    // =========================================================================

    static glm::mat4 make_model(const glm::mat4& base, const glm::vec3& t, float rot, const glm::vec3& axis, const glm::vec3& scale)
    {
        return glm::scale(glm::rotate(glm::translate(base, t), rot, axis), scale);
    }

    // =========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    // =========================================================================

    Scene::Scene(unsigned width, unsigned height)
        : angle(0.0f), moon_angle(0.0f)
    {
        skybox = std::make_shared<Skybox>("../../shared/assets/sky-cube-map-");
        load_textures();

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

        glUniform1i(glGetUniformLocation(program_id, "use_texture"), 1);
        glUniform1i(glGetUniformLocation(program_id, "diffuse_map"), 0);
        glUniform1f(glGetUniformLocation(program_id, "uv_scale"), 1.0f);
        glUniform1i(glGetUniformLocation(program_id, "is_terrain"), 0); // Falso por defecto

        resize(width, height);

        terrain_node = std::make_shared<Model>();
        earth_node = std::make_shared<Model>();
        moon_node = std::make_shared<Model>();

        terrain_node->transform.position = TERRAIN_OFFSET;
        earth_node->transform.position = MAIN_CUBE_POS;
        earth_node->transform.scale = glm::vec3(MAIN_CUBE_SCALE);
        moon_node->transform.position = SMALL_CUBE_OFFSET;
        moon_node->transform.scale = glm::vec3(SMALL_CUBE_SCALE);

        moon_node->set_parent(earth_node.get());
        earth_node->set_parent(&root);
        terrain_node->set_parent(&root);
    }

    // =========================================================================
    // GESTIÓN DE TEXTURAS
    // =========================================================================

    void Scene::load_textures()
    {
        Texture2D::load("terrain", "../../shared/assets/Imagen.png");
        Texture2D::load("earth", "../../shared/assets/crystal_texture.png");
        Texture2D::load("luna", "../../shared/assets/crystal-texture.png");
    }

    // =========================================================================
    // ACTUALIZACIÓN DE LA LÓGICA (UPDATE)
    // =========================================================================

    void Scene::update()
    {
        angle += 0.01f;
        earth_node->transform.rotation.y = angle * 0.5f;

        float moon_speed = 0.0012f;
        moon_angle += moon_speed;

        moon_node->transform.position = {
            cos(moon_angle) * 4.0f,
            0.0f,
            sin(moon_angle) * 4.0f
        };

        moon_node->transform.rotation.x = moon_angle * 2.0f;
    }

    // =========================================================================
    // RENDERIZADO DE LA ESCENA (RENDER)
    // =========================================================================

    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox->render(camera);

        glUseProgram(program_id);
        glm::mat4 view = camera.get_view_matrix();

        glUniform3f(glGetUniformLocation(program_id, "light_pos"), 10.f, 10.f, 10.f);
        glUniform3f(glGetUniformLocation(program_id, "view_pos"), camera.getX(), camera.getY(), camera.getZ());

        root.traverse(0.016f);

        int useTexLoc = glGetUniformLocation(program_id, "use_texture");
        int uvScaleLoc = glGetUniformLocation(program_id, "uv_scale");
        int isTerrainLoc = glGetUniformLocation(program_id, "is_terrain"); // Localización del flag de terreno

        // ---------------------------------------------------------------------
        // RENDER: TERRENO
        // ---------------------------------------------------------------------
        glUniform1i(isTerrainLoc, 1); // ACTIVAMOS el tinte verde en el shader

        auto terrainTex = Texture2D::get("terrain");
        if (terrainTex)
        {
            glActiveTexture(GL_TEXTURE0);
            terrainTex->bind(0);
            glUniform1i(useTexLoc, 1);
            glUniform1f(uvScaleLoc, 8.0f);
        }
        else
        {
            glUniform1i(useTexLoc, 0);
        }

        glm::mat4 terrain_model = glm::translate(glm::mat4(1.0f), TERRAIN_OFFSET);
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(view * terrain_model));
        terrain.Draw();

        glBindTexture(GL_TEXTURE_2D, 0);

        // ---------------------------------------------------------------------
        // RENDER: CUBO GRANDE (TIERRA)
        // ---------------------------------------------------------------------
        glUniform1i(isTerrainLoc, 0); // DESACTIVAMOS el tinte verde para los cubos

        auto earthTex = Texture2D::get("earth");
        if (earthTex)
        {
            cube.set_texture(earthTex->get_id());
            cube.enable_texture(true);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, earthTex->get_id());

            glUniform1i(useTexLoc, 1);
            glUniform1f(uvScaleLoc, 1.0f);
        }

        glm::mat4 main_cube_model = make_model(
            glm::mat4(1.0f),
            MAIN_CUBE_POS,
            angle * 0.5f,
            { 0, 1, 0 },
            glm::vec3(MAIN_CUBE_SCALE)
        );

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(view * main_cube_model));
        cube.set_color(glm::vec4(1.0f));
        cube.render();

        glBindTexture(GL_TEXTURE_2D, 0);

        // ---------------------------------------------------------------------
        // RENDER: CUBO PEQUEÑO (LUNA)
        // ---------------------------------------------------------------------
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // antes de renderizar el cubo transparente

        glm::mat4 small_cube_model = glm::translate(glm::mat4(1.0f), SMALL_CUBE_BASE_POS);
        small_cube_model = glm::rotate(small_cube_model, angle * 1.f, { 0, 1, 0 });
        small_cube_model = glm::translate(small_cube_model, SMALL_CUBE_OFFSET);
        small_cube_model = glm::rotate(small_cube_model, angle * 2.f, { 1, 1, 0 });
        small_cube_model = glm::scale(small_cube_model, glm::vec3(SMALL_CUBE_SCALE));

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(view * small_cube_model));

        cube.set_color(glm::vec4(
            0.5f + 0.5f * cos(angle * 0.4f),
            0.5f + 0.5f * sin(angle * 0.25f),
            0.5f + 0.5f * cos(angle * 0.3f),
            0.5f
        ));

        auto moonTex = Texture2D::get("luna");
        if (moonTex)
        {
            cube.set_texture(moonTex->get_id());
            cube.enable_texture(true);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, moonTex->get_id());

            glUniform1i(useTexLoc, 1);
            glUniform1f(uvScaleLoc, 1.0f);
        }

        cube.render();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        root.traverse(0.0f);
    }

    // =========================================================================
    // ENTRADAS, CONTROLES Y RESPUESTA DEL SISTEMA
    // =========================================================================

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

        float x, y, z;
        camera.getDirection(x, y, z);

        glm::vec3 forward = glm::normalize(glm::vec3(x, y, z));
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        float speed = 5.0f * dt;

        if (fabs(dy) > 1.0f)
            camera.setPosition(camera.getX() + forward.x * speed,
                camera.getY() + forward.y * speed,
                camera.getZ() + forward.z * speed);

        if (fabs(dx) > 1.0f)
            camera.setPosition(camera.getX() + right.x * speed,
                camera.getY() + right.y * speed,
                camera.getZ() + right.z * speed);
    }

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

    // =========================================================================
    // COMPILACIÓN Y DIAGNÓSTICO DE SHADERS
    // =========================================================================

    GLuint Scene::compile_shaders()
    {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        const char* v = vertex_shader_code.c_str();
        const char* f = fragment_shader_code.c_str();

        glShaderSource(vs, 1, &v, nullptr);
        glShaderSource(fs, 1, &f, nullptr);

        glCompileShader(vs);
        glCompileShader(fs);

        GLint success;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) show_compilation_error(vs);

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
        std::cerr << "[Shader Compilation Error]: " << log << std::endl;
        assert(false);
    }

    void Scene::show_linkage_error(GLuint id)
    {
        GLint len;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(id, len, nullptr, log.data());
        std::cerr << "[Shader Linkage Error]: " << log << std::endl;
        assert(false);
    }
}