#include "Node.hpp"

Transform transform;

void Node::set_parent(Node& node)
{
    transform.parent = &node.transform;
}