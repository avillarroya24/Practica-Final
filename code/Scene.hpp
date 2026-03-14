#pragma once

#include "Node.hpp"

class Scene
{
private:
    Node cube1;
    Node cube2;

public:
    Scene();
    void update(float time);
    void render();
};