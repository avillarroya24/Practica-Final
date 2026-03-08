#include "Scene.hpp"

#include <vector>
#include <cmath>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace glm;

namespace udit
{

    static const char* vertex_shader = R"(

#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position,1.0);
}

)";

    static const char* fragment_shader = R"(

#version 330 core

out vec4 color;

void main()
{
    color = vec4(1.0,1.0,1.0,1.0);
}

)";

    Scene::Scene(int width, int height)
    {
        shader_program = compile_shaders();

        model_id = glGetUniformLocation(shader_program, "model");
        view_id = glGetUniformLocation(shader_program, "view");
        projection_id = glGetUniformLocation(shader_program, "projection");

        generate_sphere();

        // Matrices fijas (sin cámara)
        mat4 view = lookAt(vec3(0, 0, 10), vec3(0, 0, 0), vec3(0, 1, 0));
        mat4 projection = perspective(radians(45.0f), float(width) / float(height), 0.1f, 100.0f);

        glUseProgram(shader_program);
        glUniformMatrix4fv(view_id, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, value_ptr(projection));
        glUseProgram(0);
    }


    void Scene::generate_sphere()
    {
        std::vector<float> vertices;

        const int stacks = 20;
        const int slices = 20;

        for (int i = 0; i < stacks; i++)
        {
            float lat0 = glm::pi<float>() * (-0.5f + float(i) / stacks);
            float lat1 = glm::pi<float>() * (-0.5f + float(i + 1) / stacks);

            float y0 = sin(lat0), y1 = sin(lat1);
            float r0 = cos(lat0), r1 = cos(lat1);

            for (int j = 0; j < slices; j++)
            {
                float lng0 = 2.0f * glm::pi<float>() * float(j) / slices;
                float lng1 = 2.0f * glm::pi<float>() * float(j + 1) / slices;

                float x0 = cos(lng0), z0 = sin(lng0);
                float x1 = cos(lng1), z1 = sin(lng1);

                vertices.push_back(x0 * r0); vertices.push_back(y0); vertices.push_back(z0 * r0);
                vertices.push_back(x0 * r1); vertices.push_back(y1); vertices.push_back(z0 * r1);
                vertices.push_back(x1 * r1); vertices.push_back(y1); vertices.push_back(z1 * r1);

                vertices.push_back(x0 * r0); vertices.push_back(y0); vertices.push_back(z0 * r0);
                vertices.push_back(x1 * r1); vertices.push_back(y1); vertices.push_back(z1 * r1);
                vertices.push_back(x1 * r0); vertices.push_back(y0); vertices.push_back(z1 * r0);
            }
        }

        vertex_count = vertices.size() / 3;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void Scene::update()
    {
        earth_rotation += 0.01f;
        moon_rotation += 0.02f;
        moon_orbit += 0.005f;
    }

    void Scene::render()
    {
        glUseProgram(shader_program);
        glBindVertexArray(vao);

        // Tierra
        mat4 model = rotate(mat4(1.0f), earth_rotation, vec3(0, 1, 0));
        glUniformMatrix4fv(model_id, 1, GL_FALSE, value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

        // Luna
        model = rotate(mat4(1.0f), moon_orbit, vec3(0, 1, 0));
        model = translate(model, vec3(3, 0, 0));
        model = rotate(model, moon_rotation, vec3(0, 1, 0));
        model = scale(model, vec3(0.3f));
        glUniformMatrix4fv(model_id, 1, GL_FALSE, value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

        glBindVertexArray(0);
    }

    GLuint Scene::compile_shaders()
    {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vs, 1, &vertex_shader, NULL);
        glShaderSource(fs, 1, &fragment_shader, NULL);

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