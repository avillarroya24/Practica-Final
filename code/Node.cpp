#include "Node.hpp"
#include <algorithm> // std::remove

void Node::set_parent(Node& node)
{
    if (parent)
    {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    parent = &node;
    transform.parent = &node.transform;
    node.children.push_back(this);
}

void Node::add_child(Node& child)
{
    child.set_parent(*this);
}

glm::mat4 Node::get_global_transform() const
{
    return transform.get_transform_matrix();
}

const std::vector<Node*>& Node::get_children() const
{
    return children;
}