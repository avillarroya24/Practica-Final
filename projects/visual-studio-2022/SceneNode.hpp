#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.hpp"

class SceneNode
{
public:
    glm::mat4 local;
    Mesh* mesh;
    GLuint texture;
    glm::vec4 color;
    bool transparent;

    std::vector<SceneNode*> children;

    SceneNode();
};
