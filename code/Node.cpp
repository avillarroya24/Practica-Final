#include "Node.hpp"

// Asignar padre de forma segura
void Node::set_parent(Node* node)
{
    parent = node;

    if (node)
    {
        auto it = std::find(node->children.begin(), node->children.end(), this);
        if (it == node->children.end())
            node->children.push_back(this);

        transform.parent = &node->transform;
    }
    else
    {
        transform.parent = nullptr;
    }
}

// Recorrido del grafo de escena (DFS)
void Node::traverse(float deltaTime)
{
    update(deltaTime);
    render();

    for (Node* child : children)
    {
        if (child)
            child->traverse(deltaTime);
    }
}

void Node::add_child(Node* child)
{
    if (child)
        child->set_parent(this);
}