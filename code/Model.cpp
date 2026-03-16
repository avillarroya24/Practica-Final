#include "Model.hpp"

void Model::render() const
{
    if (mesh)
        mesh->render();
}