#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Node.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

using namespace std;

class Model : public Node
{
public:

    shared_ptr<Mesh> mesh;
    shared_ptr<Material> material;

public:

    Model();

    void render(GLint model_view_matrix_id);
};