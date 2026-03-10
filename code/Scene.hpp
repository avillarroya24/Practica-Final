#pragma once

#include <memory>

#include "Cube.hpp"
#include "Model.hpp"
#include "Material.hpp"

using namespace std;

class Scene
{
    shared_ptr<Cube> cube_mesh;
    shared_ptr<Material> material;

    Model cube1;
    Model cube2;

    GLint model_view_matrix_id;
    GLint projection_matrix_id;

public:

    Scene();

    void update(float time);

    void render();
};