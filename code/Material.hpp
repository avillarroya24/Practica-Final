#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include "Shader_Program.hpp" // Asegúrate de que el nombre del archivo coincida
#include "Texture2D.hpp"

namespace udit { class Texture2D; } // Forward declaration por si acaso

class Material
{
private:
    std::shared_ptr<Shader_Program> shader_program;
    std::shared_ptr<udit::Texture2D> texture;

public:
    // Constructor con shader obligatorio
    Material(std::shared_ptr<Shader_Program> shader);

    // Getters y Setters
    void set_texture(std::shared_ptr<udit::Texture2D> tex);
    std::shared_ptr<Shader_Program> get_shader_program() const { return shader_program; }

    // Aplica el shader y vincula la textura (si existe)
    void use();
};

#endif