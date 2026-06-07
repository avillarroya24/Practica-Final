#ifndef MODEL_HPP
#define MODEL_HPP

#include "Node.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include <memory>
#include <glm.hpp>

namespace udit
{
    class Model : public Node
    {
    public:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;


        // Renderizado del grafo
        void render() override;

        // Permiten que el código antiguo funcione con la nueva estructura
        void set_texture(std::shared_ptr<Texture2D> tex);
        void enable_texture(bool enable);
        void set_color(const glm::vec4& color);
    };
}

#endif