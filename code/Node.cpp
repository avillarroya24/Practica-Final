#include "Node.hpp"

void Node::set_parent(Node* node)
{
    parent = node;

    if (node)
    {
        node->children.push_back(this);
        transform.parent = &node->transform;
    }
}

void Node::add_child(Node* child)
{
    if (!child) return;

    children.push_back(child);
    child->parent = this;
    child->transform.parent = &transform;
}