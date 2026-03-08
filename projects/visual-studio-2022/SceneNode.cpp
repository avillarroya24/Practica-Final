#include "SceneNode.hpp"
#include <glm/glm.hpp>

SceneNode::SceneNode()
{
    local = glm::mat4(1);
    mesh = nullptr;
    texture = 0;
    color = { 1,1,1,1 };
    transparent = false;
}
