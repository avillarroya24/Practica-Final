#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Node.hpp"
#include "Mesh.hpp"
#include <GL/gl.h>  // OpenGL moderno

using namespace std;

class Model : public Node
{
private:
    shared_ptr<Mesh> mesh;

public:
    // Constructor vacío
    Model() = default;

    // Setter para la malla
    void set_mesh(shared_ptr<Mesh> m) { mesh = m; }

    // Renderizar el modelo
    void render(GLint model_view_matrix_id)
    {
        if (!mesh) return;

        // Obtener la matriz de transformación del nodo
        glm::mat4 transform_matrix = transform.get_transform_matrix();

        // Enviar la matriz al shader
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(transform_matrix));

        // Renderizar la malla
        mesh->render();
    }
};