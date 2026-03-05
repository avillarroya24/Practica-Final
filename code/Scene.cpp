#include "Scene.hpp"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <vector>
#include <cstdlib> // rand, srand
#include <ctime>   // time
#include <cmath>
#include <cstdio>
#include <cstddef> // offsetof


/*
    En este caso, se implementa aqui toda la logica de la escena. La escena esta compuesta de un terreno con mallas de elevacion
    en este caso son dunas del desierto y por encima le he puesto una esfera con un tipo de color triangular para que haga una apariencia poligonal.

    La esfera lo que hace es moverse de forma random por la escena de lado a lado con el limite del tamaño del terreno.

*/

namespace udit
{

    // ---------------- SHADERS ----------------
    static const char* phong_vs =
        "#version 330 core\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec3 normal_in;\n"
        "uniform mat4 MVP;\n"
        "uniform mat4 model;\n"
        "out vec3 frag_pos;\n"
        "out vec3 normal;\n"
        "void main(){"
        "    frag_pos = vec3(model * vec4(position,1.0));"
        "    normal = normalize(mat3(transpose(inverse(model))) * normal_in);"
        "    gl_Position = MVP * vec4(position,1.0);"
        "}";

    static const char* phong_fs =
        "#version 330 core\n"
        "in vec3 frag_pos;\n"
        "in vec3 normal;\n"
        "out vec4 color;\n"
        "uniform vec3 u_color;"
        "uniform vec3 u_light_pos;"
        "uniform vec3 u_view_pos;"
        "uniform float u_alpha;"
        "void main(){"
        "    vec3 ambient = 0.2 * u_color;"
        "    vec3 norm = normalize(normal);"
        "    vec3 light_dir = normalize(u_light_pos - frag_pos);"
        "    float diff = max(dot(norm, light_dir), 0.0);"
        "    vec3 diffuse = diff * u_color;"
        "    vec3 view_dir = normalize(u_view_pos - frag_pos);"
        "    vec3 reflect_dir = reflect(-light_dir, norm);"
        "    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);"
        "    vec3 specular = vec3(0.2) * spec;"
        "    vec3 col = ambient + diffuse + specular;"
        "    color = vec4(col, u_alpha);"
        "}";

    // Shader simple para sombra (color plano, sin iluminación)
    static const char* shadow_vs =
        "#version 330 core\n"
        "layout(location=0) in vec3 position;\n"
        "uniform mat4 MVP;\n"
        "void main(){"
        "    gl_Position = MVP * vec4(position, 1.0);\n"
        "}";

    static const char* shadow_fs =
        "#version 330 core\n"
        "out vec4 color;\n"
        "uniform vec4 u_shadow_color;\n"
        "void main(){"
        "    color = u_shadow_color;\n"
        "}";

    // ---------------- UTILIDADES ----------------
    static GLuint compile_program(const char* vs_src, const char* fs_src)
    {
        auto compile = [](GLenum type, const char* src)->GLuint {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            GLint ok = 0;
            glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                char log[1024];
                glGetShaderInfoLog(s, 1024, nullptr, log);
                std::fprintf(stderr, "Shader compile error: %s\n", log);
            }
            return s;
            };
        GLuint vs = compile(GL_VERTEX_SHADER, vs_src);
        GLuint fs = compile(GL_FRAGMENT_SHADER, fs_src);
        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);
        GLint ok = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetProgramInfoLog(prog, 1024, nullptr, log);
            std::fprintf(stderr, "Program link error: %s\n", log);
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        return prog;
    }

    // ---------------- GEOMETRÍA ----------------
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
    };

    static void generate_sphere(float r, int sectors, int stacks, std::vector<Vertex>& out)
    {
        out.clear();
        const float PI = 3.14159265359f;

        for (int i = 0; i < stacks; ++i)
        {
            float v0 = float(i) / stacks;
            float v1 = float(i + 1) / stacks;

            float a0 = PI * (v0 - 0.5f);
            float a1 = PI * (v1 - 0.5f);

            float y0 = r * sin(a0);
            float y1 = r * sin(a1);

            float r0 = r * cos(a0);
            float r1 = r * cos(a1);

            for (int j = 0; j < sectors; ++j)
            {
                float u0 = float(j) / sectors;
                float u1 = float(j + 1) / sectors;

                float s0 = 2 * PI * u0;
                float s1 = 2 * PI * u1;

                glm::vec3 p00(r0 * cos(s0), y0, r0 * sin(s0));
                glm::vec3 p10(r0 * cos(s1), y0, r0 * sin(s1));
                glm::vec3 p01(r1 * cos(s0), y1, r1 * sin(s0));
                glm::vec3 p11(r1 * cos(s1), y1, r1 * sin(s1));

                glm::vec3 n1 = glm::normalize(glm::cross(p01 - p00, p10 - p00));
                out.push_back({ p00, n1 });
                out.push_back({ p01, n1 });
                out.push_back({ p10, n1 });

                glm::vec3 n2 = glm::normalize(glm::cross(p11 - p01, p10 - p01));
                out.push_back({ p01, n2 });
                out.push_back({ p11, n2 });
                out.push_back({ p10, n2 });
            }
        }
    }

    static void generate_terrain(int width, int depth, std::vector<Vertex>& out)
    {
        out.clear();
        float sizeX = 1.0f;
        float sizeZ = 1.0f;

        auto h = [](float i, float j) {
            float base = 0.5f * sin(i * 0.25f) * cos(j * 0.25f);
            float detail = 0.25f * sin(j * 0.5f);
            return base + detail;
            };

        auto compute_normal = [&](int x, int z) {
            float hL = h(x - 1, z);
            float hR = h(x + 1, z);
            float hD = h(x, z - 1);
            float hU = h(x, z + 1);
            glm::vec3 n(-(hR - hL), 2.0f, -(hU - hD));
            return glm::normalize(n);
            };

        for (int z = 0; z < depth - 1; ++z) {
            for (int x = 0; x < width - 1; ++x) {
                glm::vec3 v0(x * sizeX, h(x, z), z * sizeZ);
                glm::vec3 v1((x + 1) * sizeX, h(x + 1, z), z * sizeZ);
                glm::vec3 v2(x * sizeX, h(x, z + 1), (z + 1) * sizeZ);
                glm::vec3 v3((x + 1) * sizeX, h(x + 1, z + 1), (z + 1) * sizeZ);

                glm::vec3 n0 = compute_normal(x, z);
                glm::vec3 n1 = compute_normal(x + 1, z);
                glm::vec3 n2 = compute_normal(x, z + 1);
                glm::vec3 n3 = compute_normal(x + 1, z + 1);

                out.push_back({ v0, n0 });
                out.push_back({ v2, n2 });
                out.push_back({ v3, n3 });
                out.push_back({ v0, n0 });
                out.push_back({ v3, n3 });
                out.push_back({ v1, n1 });
            }
        }
    }

    // ---------------- VARIABLES ----------------
    static GLuint g_prog = 0;
    static GLuint g_shadow_prog = 0;
    static GLuint g_sphere_vao = 0, g_sphere_vbo = 0;
    static GLsizei g_sphere_count = 0;
    static GLuint g_base_vao = 0, g_base_vbo = 0;
    static GLsizei g_base_count = 0;

    // Movimiento de la esfera
    static float g_sphere_time = 1.0f;
    static glm::vec2 g_sphere_pos_2d(0.f, -2.5f); // posición X,Z en el terreno
    static glm::vec2 g_sphere_target_2d(0.f, -2.5f); // objetivo actual
    static float g_target_timer = 0.0f; // tiempo restante hasta elegir nuevo objetivo
    static const float g_target_interval = 2.0f; // segundos entre objetivos
    static const float g_sphere_scale = 2.6f;     // escala del modelo de la esfera

    // ---------------- FUNCIONES AUXILIARES ----------------
    static float terrain_height(float x, float z)
    {
        float base = 0.5f * sin(x * 0.25f) * cos(z * 0.25f);
        float detail = 0.25f * sin(z * 0.5f);
        return base + detail;
    }

    static glm::mat4 shadow_matrix(const glm::vec4& plane, const glm::vec3& light)
    {
        glm::vec4 L(light, 1.0f);
        float dot = glm::dot(plane, L);

        glm::mat4 S(0.0f);
        S[0][0] = dot - L.x * plane.x;
        S[1][0] = -L.x * plane.y;
        S[2][0] = -L.x * plane.z;
        S[3][0] = -L.x * plane.w;

        S[0][1] = -L.y * plane.x;
        S[1][1] = dot - L.y * plane.y;
        S[2][1] = -L.y * plane.z;
        S[3][1] = -L.y * plane.w;

        S[0][2] = -L.z * plane.x;
        S[1][2] = -L.z * plane.y;
        S[2][2] = dot - L.z * plane.z;
        S[3][2] = -L.z * plane.w;

        S[0][3] = -plane.x;
        S[1][3] = -plane.y;
        S[2][3] = -plane.z;
        S[3][3] = dot - L.w * plane.w;

        return S;
    }

    // Escoge un punto objetivo aleatorio dentro de los límites
    static glm::vec2 pick_random_target(float limit)
    {
        float rx = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f; // -1..1
        float rz = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
        return glm::vec2(rx * limit, rz * limit);
    }

    // ---------------- CONSTRUCTOR ----------------
    Scene::Scene(int width, int height)
        : skybox("../../../shared/assets/sky-cube-map-")
    {
        glEnable(GL_DEPTH_TEST);
        resize(width, height);

        angle_around_x = angle_delta_x = 0.0f;
        angle_around_y = angle_delta_y = 0.0f;
        pointer_pressed = false;

        camera.set_location(0.f, 4.2f, 10.0f);
        camera.set_target(0.f, 4.2f, -2.5f);

        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        g_prog = compile_program(phong_vs, phong_fs);
        g_shadow_prog = compile_program(shadow_vs, shadow_fs);

        std::vector<Vertex> sphere;
        generate_sphere(1.0f, 48, 24, sphere);
        g_sphere_count = static_cast<GLsizei>(sphere.size());
        glGenVertexArrays(1, &g_sphere_vao);
        glGenBuffers(1, &g_sphere_vbo);
        glBindVertexArray(g_sphere_vao);
        glBindBuffer(GL_ARRAY_BUFFER, g_sphere_vbo);
        glBufferData(GL_ARRAY_BUFFER, sphere.size() * sizeof(Vertex), sphere.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glBindVertexArray(0);

        std::vector<Vertex> terrain;
        generate_terrain(30, 30, terrain);
        g_base_count = static_cast<GLsizei>(terrain.size());
        glGenVertexArrays(1, &g_base_vao);
        glGenBuffers(1, &g_base_vbo);
        glBindVertexArray(g_base_vao);
        glBindBuffer(GL_ARRAY_BUFFER, g_base_vbo);
        glBufferData(GL_ARRAY_BUFFER, terrain.size() * sizeof(Vertex), terrain.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glBindVertexArray(0);

        // Inicializar objetivo y temporizador
        float limit = 30.0f * 2.0f;
        g_sphere_target_2d = pick_random_target(limit);
        g_target_timer = g_target_interval;
    }

    // ---------------- UPDATE ----------------
    void Scene::update()
    {
        angle_around_x += angle_delta_x;
        angle_around_y += angle_delta_y;

        if (angle_around_x < -1.5f) angle_around_x = -1.5f;
        if (angle_around_x > 1.5f) angle_around_x = 1.5f;

        glm::mat4 camera_rotation(1.f);
        camera_rotation = glm::rotate(camera_rotation, angle_around_y, glm::vec3(0.f, 1.f, 0.f));
        camera_rotation = glm::rotate(camera_rotation, angle_around_x, glm::vec3(1.f, 0.f, 0.f));
        camera.set_target(0.f, 0.f, -1.f);
        camera.rotate(camera_rotation);

        // Tiempo
        const float dt = 0.016f; // aproximación de frame time
        g_sphere_time += dt;

        // Actualizar temporizador de objetivo
        g_target_timer -= dt;
        float limit = 30.0f * 2.0f;

        if (g_target_timer <= 0.0f) {
            // Elegir nuevo objetivo aleatorio
            g_sphere_target_2d = pick_random_target(limit);
            g_target_timer = g_target_interval + (static_cast<float>(std::rand()) / RAND_MAX) * 1.5f; // intervalo variable
        }

        // Movimiento suave hacia el objetivo (steering)
        glm::vec2 to_target = g_sphere_target_2d - g_sphere_pos_2d;
        float dist = glm::length(to_target);
        if (dist > 0.001f) {
            glm::vec2 dir = to_target / dist;
            // velocidad dependiente de la pendiente del terreno local (opcional)
            float speed = 4.0f * dt; // unidades por segundo aproximadas (ajustar si es necesario)
            // Si estamos cerca, reducir velocidad para suavizar la parada
            if (dist < 1.0f) speed *= (dist / 1.0f);
            g_sphere_pos_2d += dir * speed;
        }
        else {
            // ya en objetivo: esperar a que el temporizador elija otro
        }

        // Mantener dentro de límites
        if (g_sphere_pos_2d.x < -limit) g_sphere_pos_2d.x = -limit;
        if (g_sphere_pos_2d.x > limit) g_sphere_pos_2d.x = limit;
        if (g_sphere_pos_2d.y < -limit) g_sphere_pos_2d.y = -limit;
        if (g_sphere_pos_2d.y > limit) g_sphere_pos_2d.y = limit;
    }

    // ---------------- RENDER ----------------
    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skybox.render(camera);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        glm::mat4 proj = camera.get_projection_matrix();
        glm::mat4 view = camera.get_transform_matrix_inverse();

        // ---------------- UNIFORMS ----------------
        glUseProgram(g_prog);
        GLint loc_mvp = glGetUniformLocation(g_prog, "MVP");
        GLint loc_model = glGetUniformLocation(g_prog, "model");
        GLint loc_color = glGetUniformLocation(g_prog, "u_color");
        GLint loc_light = glGetUniformLocation(g_prog, "u_light_pos");
        GLint loc_view = glGetUniformLocation(g_prog, "u_view_pos");
        GLint loc_alpha = glGetUniformLocation(g_prog, "u_alpha");

        glm::vec3 light_pos(5.0f, 8.0f, 5.0f);
        glm::vec3 view_pos = camera.get_location();

        // ---------------- TERRENO ----------------
        glBindVertexArray(g_base_vao);
        const int tile_range = 2;
        float tile_size = 30.0f;
        for (int dz = -tile_range; dz <= tile_range; ++dz)
            for (int dx = -tile_range; dx <= tile_range; ++dx)
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(dx * tile_size, 0.f, dz * tile_size));
                float scale_x = 2.0f * width / float(width > height ? width : height);
                float scale_z = 2.0f * height / float(width > height ? width : height);
                model = glm::scale(model, glm::vec3(scale_x, 1.0f, scale_z));
                glm::mat4 mvp = proj * view * model;

                glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3f(loc_color, 0.9f, 0.85f, 0.7f);
                glUniform3f(loc_light, light_pos.x, light_pos.y, light_pos.z);
                glUniform3f(loc_view, view_pos.x, view_pos.y, view_pos.z);
                glUniform1f(loc_alpha, 1.0f);

                glDrawArrays(GL_TRIANGLES, 0, g_base_count);
            }
        glBindVertexArray(0);

        // ---------------- ESFERA ----------------
        float terrain_y = terrain_height(g_sphere_pos_2d.x, g_sphere_pos_2d.y);
        float sphere_world_y = terrain_y + (1.0f * g_sphere_scale) + 0.01f;
        glm::vec3 sphere_pos(g_sphere_pos_2d.x, sphere_world_y, g_sphere_pos_2d.y);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), sphere_pos);
        model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(g_sphere_scale));
        glm::mat4 mvp = proj * view * model;

        glBindVertexArray(g_sphere_vao);
        glUseProgram(g_prog);
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(loc_color, 0.82f, 0.78f, 0.7f);
        glUniform3f(loc_light, light_pos.x, light_pos.y, light_pos.z);
        glUniform3f(loc_view, view_pos.x, view_pos.y, view_pos.z);
        glUniform1f(loc_alpha, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, g_sphere_count);

        // ---------------- SOMBRA REALISTA ----------------
        float y_ground = terrain_y;
        glm::vec4 ground_plane(0.0f, 1.0f, 0.0f, -y_ground);
        glm::mat4 S = shadow_matrix(ground_plane, light_pos);
        glm::mat4 shadow_model = S * model;
        glm::mat4 shadow_mvp = proj * view * shadow_model;

        glUseProgram(g_shadow_prog);
        GLint loc_mvp_shadow = glGetUniformLocation(g_shadow_prog, "MVP");
        GLint loc_shadow_color = glGetUniformLocation(g_shadow_prog, "u_shadow_color");

        glUniformMatrix4fv(loc_mvp_shadow, 1, GL_FALSE, glm::value_ptr(shadow_mvp));
        glUniform4f(loc_shadow_color, 0.0f, 0.0f, 0.0f, 0.45f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);

        glDrawArrays(GL_TRIANGLES, 0, g_sphere_count);

        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_BLEND);

        glBindVertexArray(0);
    }

    // ---------------- RESIZE / INPUT ----------------
    void Scene::resize(int new_width, int new_height)
    {
        width = new_width;
        height = new_height;
        camera.set_ratio(float(width) / height);
        glViewport(0, 0, width, height);
    }

    void Scene::on_drag(float pointer_x, float pointer_y)
    {
        if (pointer_pressed) {
            angle_delta_x = 0.025f * (last_pointer_y - pointer_y) / height;
            angle_delta_y = 0.025f * (last_pointer_x - pointer_x) / width;
        }
    }

    void Scene::on_click(float pointer_x, float pointer_y, bool down)
    {
        if ((pointer_pressed = down) == true) {
            last_pointer_x = pointer_x;
            last_pointer_y = pointer_y;
        }
        else {
            angle_delta_x = angle_delta_y = 0.0f;
        }
    }

} // namespace udit
