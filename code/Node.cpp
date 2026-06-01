#include "Node.hpp"

/*

   Implementación de la clase Node.
 
 * Este archivo define el comportamiento de un nodo dentro del grafo de escena.
 * Un nodo representa una entidad jerárquica que puede tener un padre y múltiples hijos,
   permitiendo estructurar la escena mediante una jerarquía (scene graph).
*/

// ================= ASIGNACIÓN DE PADRE =================

/*

   Asigna el nodo padre de forma segura.
 
 * Este método:
 * - Establece el nodo padre
 * - Añade el nodo actual a la lista de hijos del padre si no está ya
 * - Actualiza la jerarquía de transformaciones
 
 * @param node Nodo padre al que se va a asociar este nodo.

*/
void Node::set_parent(Node* node)
{
    parent = node;

    if (node)
    {
        auto it = std::find(node->children.begin(), node->children.end(), this); //Evita duplicados en la lista de hijos del padre
        if (it == node->children.end())
            node->children.push_back(this);

        transform.parent = &node->transform; //La transformación pasa a depender del padre
    }
    else
    {
        transform.parent = nullptr;  //Sin padre, la transformación es independiente
    }
}

// ================= RECORRIDO DEL GRAFO =================

/*

   Recorre el grafo de escena en profundidad (DFS).
 
 * Ejecuta la actualización y render del nodo actual y después
   recorre recursivamente todos sus hijos.
 
 * @param deltaTime Tiempo entre frames para actualizar lógica.

*/
void Node::traverse(float deltaTime)
{
    update(deltaTime);//Actualiza lógica del nodo actual
    render();

    for (Node* child : children)//Recorre los hijos recursivamente
    {
        if (child)
            child->traverse(deltaTime);
    }
}


// ================= GESTIÓN DE HIJOS =================

/*

   Añade un nodo hijo.
 
 * El hijo se vincula automáticamente como descendiente del nodo actual.
 
 * @param child Nodo hijo a añadir.

*/

void Node::add_child(Node* child)
{
    if (child)
        child->set_parent(this);
}