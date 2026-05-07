#ifndef NODE_HPP
#define NODE_HPP

#include "Transform.hpp"
#include <vector>

class Node
{
public:
    Transform transform;

    Node* parent = nullptr;
    std::vector<Node*> children;

    virtual ~Node() {}

    // IMPORTANTE: pueden ser default si no quieres override obligatorio
    virtual void update(float deltaTime) {}
    virtual void render() {}
    void traverse(float deltaTime);

    void set_parent(Node* node);
    void add_child(Node* child);
};

#endif