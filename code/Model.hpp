#ifndef MODEL_HPP
#define MODEL_HPP

#include "Node.hpp"
#include "Mesh.hpp"
#include <memory>

class Model : public Node
{
public:
    std::shared_ptr<Mesh> mesh;

    void render() const
    {
        if (mesh)
            mesh->render();
    }
};

#endif