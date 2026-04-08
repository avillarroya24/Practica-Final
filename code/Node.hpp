#ifndef NODE_HPP
#define NODE_HPP

#include "Tranform.hpp"
#include "Light.hpp"

class Node
{
public:
    Transform transform;

    virtual void update(float deltaTime) {}
    virtual void render() {}

    void set_parent(Node& node);
};

#endif