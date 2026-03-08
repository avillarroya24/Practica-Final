#include "Scene.hpp"
#include "Skybox.hpp"
#include <gtc/type_ptr.hpp>

using namespace glm;

namespace udit
{
    static const char* vertex_shader = R"(

    #version 330 core
    layout(location = 0) in vec3 position;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0);
    }

    )";

    static const char* fragment_shader = R"(

    #version 330 core
    out vec4 color;
    void main()
    {
        color = vec4(1.0, 1.0, 1.0, 1.0);
    }

    )";

    Scene::Scene(int width, int height)
        : width(width), height(height)
    {
        shader_program = compile_shaders();

        model_id = glGetUniformLocation(shader_program, "model");
        view_id = glGetUniformLocation(shader_program, "view");
        projection_id = glGetUniformLocation(shader_program, "projection");

        generate_sphere();

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
    }

    Scene::~Scene()
    {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ebo) glDeleteBuffers(1, &ebo);
        if (shader_program) glDeleteProgram(shader_program);
    }

    void Scene::generate_sphere()
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        const int stacks = 20;
        const int slices = 20;

        // -------------------------
        // GENERAR VÉRTICES
        // -------------------------
        for (int i = 0; i <= stacks; i++)
        {
            float lat = pi<float>() * (-0.5f + (float)i / stacks);
            float y = sin(lat);
            float r = cos(lat);

            for (int j = 0; j <= slices; j++)
            {
                float lng = 2.0f * pi<float>() * (float)j / slices;
                float x = cos(lng) * r;
                float z = sin(lng) * r;

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        // -------------------------
        // GENERAR ÍNDICES (wireframe)
        // -------------------------
        for (int i = 0; i < stacks; i++)
        {
            for (int j = 0; j < slices; j++)
            {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;

                // Línea horizontal
                indices.push_back(first);
                indices.push_back(first + 1);

                // Línea vertical
                indices.push_back(first);
                indices.push_back(second);
            }
        }

        vertex_count = indices.size();

        // -------------------------
        // CREAR VAO / VBO / EBO
        // -------------------------
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
            vertices.size() * sizeof(float),
            vertices.data(),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void Scene::update()
    {
        earth_rotation += 0.01f;
        moon_rotation += 0.02f;
        moon_orbit += 0.01f;
    }

    void Scene::render()
    {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        mat4 view = lookAt(vec3(0, 2, 6), vec3(0, 0, 0), vec3(0, 1, 0));
        mat4 projection = perspective(radians(45.0f),
            float(width) / float(height),
            0.1f, 100.0f);

        glUniformMatrix4fv(view_id, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, value_ptr(projection));

        glBindVertexArray(vao);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // -------------------------
        // ESFERA GRANDE
        // -------------------------
        mat4 model = rotate(mat4(1.0f), earth_rotation, vec3(0, 1, 0));
        model = scale(model, vec3(1.5f));
        glUniformMatrix4fv(model_id, 1, GL_FALSE, value_ptr(model));
        glDrawElements(GL_LINES, vertex_count, GL_UNSIGNED_INT, 0);

        // -------------------------
        // ESFERA PEQUEÑA
        // -------------------------
        mat4 moon_model = rotate(mat4(1.0f), moon_orbit, vec3(0, 1, 0));
        moon_model = translate(moon_model, vec3(3, 0, 0));
        moon_model = rotate(moon_model, moon_rotation, vec3(0, 1, 0));
        moon_model = scale(moon_model, vec3(0.4f));

        glUniformMatrix4fv(model_id, 1, GL_FALSE, value_ptr(moon_model));
        glDrawElements(GL_LINES, vertex_count, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    GLuint Scene::compile_shaders()
    {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vs, 1, &vertex_shader, nullptr);
        glShaderSource(fs, 1, &fragment_shader, nullptr);

        glCompileShader(vs);
        glCompileShader(fs);

        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

}
