#pragma once

#include "Transform.hpp"

class Node
{
public:

    Transform transform;

    Node();

    void set_parent(Node& node);
};