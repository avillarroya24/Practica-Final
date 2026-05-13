#ifndef NODE_HPP
#define NODE_HPP

#include "Transform.hpp"
#include <vector>
#include <algorithm> // Para std::find

class Node
{
public:
    Transform transform;

    Node* parent = nullptr;
    std::vector<Node*> children;

    virtual ~Node() {}

    // Estos métodos se sobreescriben en Model, Camera, etc.
    virtual void update(float deltaTime) {}
    virtual void render() {}

    // El motor del grafo
    void traverse(float deltaTime);

    void set_parent(Node* node);
    void add_child(Node* child);
};

#endif