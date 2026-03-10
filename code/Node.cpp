#include "Node.hpp"

Node::Node()
{
}

void Node::set_parent(Node& node)
{
    transform.parent = &node.transform;
}