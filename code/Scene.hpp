#pragma once

#include <memory>
#include <GL/glew.h>

#include "Cube.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "Shader_Program.hpp"

class Scene
{
    std::shared_ptr<Cube> cube_mesh;
    std::shared_ptr<Material> material;

    Model cube1;
    Model cube2;

    GLint model_view_matrix_id;
    GLint projection_matrix_id;

public:

    Scene();

    void update(float time);
    void render();
};
