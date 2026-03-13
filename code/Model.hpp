#pragma once

#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl.h>

#include "Node.hpp"

class Mesh;
class Material;

class Model : public Node
{
public:

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    void render(GLint model_view_matrix_id);
};
