#pragma once
#include <memory>
#include <glm/gl.hpp>
#include "Cube.hpp"

using namespace std;
using namespace glm;

class Scene
{
private:
    shared_ptr<Cube> cube_mesh;
    shared_ptr<Material> material;

    Model cube1;
    Model cube2;

    GLint model_view_matrix_id;
    GLint projection_matrix_id;

public:
    Scene();
    ~Scene() = default;

    void render();
};