#ifndef NODE_HPP
#define NODE_HPP

#include "Tranform.hpp"

class Node
{
public:
    Transform transform;

    void set_parent(Node& node);
};

#endif