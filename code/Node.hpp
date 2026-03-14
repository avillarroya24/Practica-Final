#pragma once

struct Transform
{
    float position[3] = { 0,0,0 };
    float rotation[3] = { 0,0,0 };
    float scale[3] = { 1,1,1 };
};

class Node
{
public:
    Transform transform;
    Node* parent = nullptr;

    void set_parent(Node* p) { parent = p; }

    // render vacío para evitar errores
    void render() {}
};