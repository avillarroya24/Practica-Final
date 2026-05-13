#include "Node.hpp"

// Asignar padre de forma segura
void Node::set_parent(Node* node)
{
    // Si ya teníamos un padre, deberíamos desvincularnos (opcional para nota)
    parent = node;

    if (node)
    {
        // Evitamos duplicados en la lista de hijos del padre
        auto it = std::find(node->children.begin(), node->children.end(), this);
        if (it == node->children.end()) {
            node->children.push_back(this);
        }

        // CLAVE DEL GRAFO: Conexión de transform jerárquico
        // Ahora, cuando este nodo pida su matriz, subirá hasta el padre raíz
        transform.parent = &node->transform;
    }
    else
    {
        transform.parent = nullptr;
    }
}

// Recorrido del grafo de escena (Depth-First Search)
void Node::traverse(float deltaTime)
{
    // 1. Lógica: Se llama al update del objeto (ej: rotar la tierra)
    update(deltaTime);

    // 2. Dibujado: Se llama al render del objeto (ej: Model::render dibuja el mesh con su textura)
    render();

    // 3. Propagación: Se recorren los hijos (ej: la luna se dibuja después de la tierra)
    for (Node* child : children)
    {
        if (child)
            child->traverse(deltaTime);
    }
}

// Añadir hijo (simplemente redirige a set_parent para mantener coherencia)
void Node::add_child(Node* child)
{
    if (child) {
        child->set_parent(this);
    }
}