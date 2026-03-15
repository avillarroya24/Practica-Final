#pragma once

#include "Transform.hpp"
#include <vector>

class Node
{
public:
    Transform transform;

private:
    Node* parent = nullptr;
    std::vector<Node*> children;

public:
    Node() = default;

    // Declaraciones SIN cuerpo
    void set_parent(Node& node);
    void add_child(Node& child);
    glm::mat4 get_global_transform() const;
    const std::vector<Node*>& get_children() const;
};