#include "Scene.hpp"

#include <vector>
#include <cmath>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace udit;

Scene::Scene(int width, int height)
{

    this->width = width;
    this->height = height;

    bigRotation = 0.0f;
    smallOrbit = 0.0f;
    smallRotation = 0.0f;

    glEnable(GL_DEPTH_TEST);

    resize(width, height);

    std::vector<float> vertices;

    int stacks = 20;
    int slices = 20;

    for (int i = 0; i < stacks; i++)
    {
        float lat0 = M_PI * (-0.5 + (float)i / stacks);
        float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);

        float z0 = sin(lat0);
        float zr0 = cos(lat0);

        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        for (int j = 0; j < slices; j++)
        {
            float lng0 = 2 * M_PI * (float)(j) / slices;
            float lng1 = 2 * M_PI * (float)(j + 1) / slices;

            float x0 = cos(lng0);
            float y0 = sin(lng0);

            float x1 = cos(lng1);
            float y1 = sin(lng1);

            vertices.push_back(x0 * zr0);
            vertices.push_back(z0);
            vertices.push_back(y0 * zr0);

            vertices.push_back(x0 * zr1);
            vertices.push_back(z1);
            vertices.push_back(y0 * zr1);

            vertices.push_back(x1 * zr1);
            vertices.push_back(z1);
            vertices.push_back(y1 * zr1);

            vertices.push_back(x0 * zr0);
            vertices.push_back(z0);
            vertices.push_back(y0 * zr0);

            vertices.push_back(x1 * zr1);
            vertices.push_back(z1);
            vertices.push_back(y1 * zr1);

            vertices.push_back(x1 * zr0);
            vertices.push_back(z0);
            vertices.push_back(y1 * zr0);
        }
    }

    vertexCount = vertices.size() / 3;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void Scene::update()
{
    bigRotation += 0.01f;
    smallOrbit += 0.02f;
    smallRotation += 0.05f;
}

void Scene::render()
{

    glClearColor(0.2f, 0.3f, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)width / height, 0.1f, 100.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0, 3, 8),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    glBindVertexArray(vao);

    // ESFERA GRANDE

    glm::mat4 model = glm::mat4(1);

    model = glm::rotate(model, bigRotation, glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(1.5));

    glm::mat4 mvp = proj * view * model;

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    // ESFERA PEQUEÑA

    glm::mat4 small = glm::mat4(1);

    small = glm::rotate(small, smallOrbit, glm::vec3(0, 1, 0));
    small = glm::translate(small, glm::vec3(3, 0, 0));
    small = glm::rotate(small, smallRotation, glm::vec3(0, 1, 0));
    small = glm::scale(small, glm::vec3(0.5));

    glm::mat4 mvp2 = proj * view * small;

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp2));

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void Scene::resize(int new_width, int new_height)
{

    width = new_width;
    height = new_height;

    glViewport(0, 0, width, height);
}