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

/*

   Implementación de la clase Scene.
 
 * Este archivo contiene la lógica principal del motor gráfico:
 * - Gestión de shaders
 * - Inicialización de objetos 3D
 * - Actualización de la escena
 * - Renderizado completo
 * - Gestión de cámara, luces y skybox


*/



namespace udit
{
    // ================= SHADERS =================

    /*
    
       Vertex shader del motor.
     
     * Calcula la posición de los vértices en pantalla y pasa información
     * al fragment shader (normales, UVs, posición, etc.).
    
    */

    /*
    
       Fragment shader del motor.
     
     * Calcula iluminación (ambient, diffuse, specular) y aplica textura
       si está activada.
    
    */

    const std::string Scene::vertex_shader_code =
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

    const std::string Scene::fragment_shader_code =
        "#version 330 core\n"
        "in vec3 frag_color;\n"
        "in vec3 normal;\n"
        "in vec3 frag_pos;\n"
        "in vec2 texcoord;\n"
        "out vec4 fragment_color;\n"
        "uniform sampler2D diffuse_map;\n"
        "uniform bool use_texture;\n"
        "uniform float uv_scale;\n"
        "uniform vec3 light_pos;\n"
        "uniform vec3 view_pos;\n"
        "void main() {\n"
        "    vec3 norm = normalize(normal);\n"
        "    vec3 light_dir = normalize(light_pos - frag_pos);\n"
        "    float ambient = 0.2;\n"
        "    vec3 ambient_col = ambient * frag_color;\n"
        "    float diff = max(dot(norm, light_dir), 0.0);\n"
        "    vec3 diffuse = diff * frag_color;\n"
        "    vec3 view_dir = normalize(view_pos - frag_pos);\n"
        "    vec3 reflect_dir = reflect(-light_dir, norm);\n"
        "    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);\n"
        "    vec3 specular = 0.7 * spec * vec3(1.0);\n"
        "    vec3 result = pow(ambient_col + diffuse + specular, vec3(1.3));\n"
        "    float alpha = (uv_scale < 1.0) ? 0.3 : 1.0;\n"
        "    fragment_color = use_texture ? vec4(texture(diffuse_map, texcoord).rgb, alpha)\n"
        "                                 : vec4(result, alpha);\n"
        "}";

    // ================= CONSTANTES =================

    //Namespace de constantes de configuración de escena.
    namespace
    {
        constexpr glm::vec3 TERRAIN_OFFSET(-100.f, -5.f, -200.f);
        constexpr glm::vec3 MAIN_CUBE_POS(0.f, 0.f, -6.f);
        constexpr float MAIN_CUBE_SCALE = 1.2f;

        constexpr glm::vec3 SMALL_CUBE_BASE_POS(0.f, 0.f, -6.f);
        constexpr glm::vec3 SMALL_CUBE_OFFSET(4.f, 0.f, 0.f);
        constexpr float SMALL_CUBE_SCALE = 0.35f;
    }

    // ================= UTILIDAD =================

    /*
       Construye una matriz de modelo combinando transformaciones.
    
     * @param base Matriz base.
     * @param t Traslación.
     * @param rot Rotación en radianes.
     * @param axis Eje de rotación.
     * @param scale Escala.
     * @return Matriz modelo resultante.
    
    */

    static glm::mat4 make_model(const glm::mat4& base,
        const glm::vec3& t,
        float rot,
        const glm::vec3& axis,
        const glm::vec3& scale)
    {
        return glm::scale(
            glm::rotate(
                glm::translate(base, t),
                rot,
                axis),
            scale);
    }

    // ================= CONSTRUCTOR =================

    /*
    
       Constructor de la escena.
   
     * Inicializa:
     * - Skybox
     * - Texturas
     * - OpenGL (depth, culling, blending)
     * - Shaders
     * - Cámara y objetos de la escena
    
    */
    Scene::Scene(unsigned width, unsigned height)
        : angle(0.0f)
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

        resize(width, height);

        //Creacion de los nodos del grafo de escena
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

    // ================= TEXTURAS =================
    
    //Carga las texturas del motor
    void Scene::load_textures()
    {
        texture_wood = std::make_shared<Texture2D>("../../shared/assets/wood.png");
    }

    // ================= UPDATE =================

    /*
    
       Actualiza la lógica de la escena.
    
     * Se encarga de animaciones como rotación de la tierra y órbita de la luna.
    
    */
    void Scene::update()
    {
        angle += 0.01f;

        earth_node->transform.rotation.y = angle * 0.5f;

        // ================= LUNA REALISTA =================
        // 1 vuelta completa ≈ 27.3 días → simulado lento
        float moon_speed = 0.0012f; // muy lento estilo real

        moon_angle += moon_speed;

        moon_node->transform.position = {
            cos(moon_angle) * 4.0f,
            0.0f,
            sin(moon_angle) * 4.0f
        };

        moon_node->transform.rotation.x = moon_angle * 2.0f;
    }

    void Scene::render()
    {
        // ================= LIMPIEZA DE PANTALLA =================
        // Borra color y profundidad antes de renderizar el nuevo frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ================= SKYBOX =================
        // Renderiza el fondo (cielo)
        skybox->render(camera);

        // Activar shader principal
        glUseProgram(program_id);

        // Obtener matriz de vista de la cámara
        glm::mat4 view = camera.get_view_matrix();

        // ================= LUZ Y CÁMARA =================
        // Posición de la luz en la escena
        glUniform3f(glGetUniformLocation(program_id, "light_pos"), 10.f, 10.f, 10.f);

        // Posición de la cámara (para iluminación especular)
        glUniform3f(glGetUniformLocation(program_id, "view_pos"),
            camera.getX(), camera.getY(), camera.getZ());

        // ================= GRAFO DE ESCENA =================
        // Recorre el grafo (jerarquía de nodos)
        root.traverse(0.016f);

        // Uniform para activar/desactivar texturas
        int useTexLoc = glGetUniformLocation(program_id, "use_texture");

        // =====================================================
        // TERRENO
        // =====================================================

        // Obtener textura del terreno
        auto terrainTex = Texture2D::get("terrain");

        if (terrainTex)
        {
            // Activar textura
            terrainTex->bind(0);
            glUniform1i(useTexLoc, 1);

            // Escalar UVs (repetición de textura)
            glUniform1f(glGetUniformLocation(program_id, "uv_scale"), 8.0f);
        }
        else
        {
            glUniform1i(useTexLoc, 0);
        }

        // Matriz de transformación del terreno (posición en el mundo)
        glm::mat4 terrain_model = glm::translate(glm::mat4(1.0f), TERRAIN_OFFSET);

        // Enviar matriz al shader (vista * modelo)
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE,
            glm::value_ptr(view * terrain_model));

        // Dibujar el terreno
        terrain.Draw();


        // =====================================================
        // CUBO GRANDE (TIERRA)
        // =====================================================

        // Obtener textura del cubo grande (tierra)
        auto earthTex = Texture2D::get("earth");

        if (earthTex)
        {
            // Asignar textura al cubo
            cube.set_texture(earthTex->get_id());
            cube.enable_texture(true);

            // Activar textura en OpenGL
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, earthTex->get_id());

            glUniform1i(useTexLoc, 0);
        }

        // Transformación del cubo grande:
        // - Posición en escena
        // - Rotación (simula giro de la Tierra)
        // - Escala
        glm::mat4 main_cube_model = make_model(
            glm::mat4(1.0f),
            MAIN_CUBE_POS,
            angle * 0.5f,
            { 0,1,0 },
            glm::vec3(MAIN_CUBE_SCALE)
        );

        // Enviar matriz al shader
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE,
            glm::value_ptr(view * main_cube_model));

        // Dibujar cubo grande
        cube.render();


        // =====================================================
        // CUBO PEQUEÑO
        // =====================================================

        // Activar blending para transparencia
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Desactivar escritura en profundidad (para transparencia correcta)
        glDepthMask(GL_FALSE);

        // Transformación del cubo pequeño:
        // - Parte desde base
        // - Rota alrededor del cubo grande (órbita)
        // - Se desplaza
        // - Rota sobre sí mismo
        // - Escala pequeña
        glm::mat4 small_cube_model = glm::translate(glm::mat4(1.0f), SMALL_CUBE_BASE_POS);
        small_cube_model = glm::rotate(small_cube_model, angle * 1.f, { 0,1,0 });
        small_cube_model = glm::translate(small_cube_model, SMALL_CUBE_OFFSET);
        small_cube_model = glm::rotate(small_cube_model, angle * 2.f, { 1,1,0 });
        small_cube_model = glm::scale(small_cube_model, glm::vec3(SMALL_CUBE_SCALE));

        // Enviar matriz al shader
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE,
            glm::value_ptr(view * small_cube_model));


        // Color dinámico + transparencia (alpha)
        cube.set_color(glm::vec4(
            0.5f + 0.5f * cos(angle * 0.4f),
            0.5f + 0.5f * sin(angle * 0.25f),
            0.5f + 0.5f * cos(angle * 0.3f),
            0.04f
        ));

        // Dibujar cubo pequeño
        cube.render();

        // Restaurar estado normal de profundidad
        glDepthMask(GL_TRUE);

        // Desactivar blending
        glDisable(GL_BLEND);


        // ================= GRAFO FINAL =================
        // Segunda pasada del grafo (según tu estructura actual)
        root.traverse(0.0f);
    }

    // ================= CONTROLS =================
    void Scene::moveForward(float dt) { camera.moveForward(dt); }
    void Scene::moveBackward(float dt) { camera.moveBackward(dt); }
    void Scene::moveLeft(float dt) { camera.moveLeft(dt); }
    void Scene::moveRight(float dt) { camera.moveRight(dt); }
    void Scene::moveUp(float dt) { camera.moveUp(dt); }
    void Scene::moveDown(float dt) { camera.moveDown(dt); }
    void Scene::rotateCamera(float dx, float dy) { camera.rotate(dx, dy); }


    // ================= INPUT =================

    //Gestiona entrada del ratón para cámara.

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

    // ================= RESIZE =================
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

    // ================= SHADERS =================

    /*
    
       Compila y enlaza shaders del motor.
     * @return ID del programa de shaders.
    
    */
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