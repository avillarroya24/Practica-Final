#include "Node.hpp"

void Node::set_parent(Node& node)
{
    transform.parent = &node.transform;
}