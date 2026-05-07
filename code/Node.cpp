#include "Node.hpp"

// Asignar padre
void Node::set_parent(Node* node)
{
    parent = node;

    if (node)
    {
        node->children.push_back(this);

        // Conexión de transform jerárquico
        transform.parent = &node->transform;
    }
}

// Recorrido del grafo de escena
void Node::traverse(float deltaTime)
{
    // 1. Update del nodo actual
    update(deltaTime);

    // 2. Render del nodo actual
    // IMPORTANTE: aquí ya se usa el transform global
    render();

    // 3. Recorrer hijos (propagación jerárquica)
    for (Node* child : children)
    {
        if (child)
            child->traverse(deltaTime);
    }
}

// Añadir hijo
void Node::add_child(Node* child)
{
    if (!child) return;

    children.push_back(child);

    child->parent = this;

    // CLAVE: conectar transform con el padre
    child->transform.parent = &transform;
}