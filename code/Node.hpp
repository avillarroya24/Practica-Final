#pragma once
#include "Transform.hpp"

class Node
{
public:

    Transform transform;

    void set_parent(Node& node)
    {
        transform.parent = &node.transform;
    }
};
