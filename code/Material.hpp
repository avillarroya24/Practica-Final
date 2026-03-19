#ifndef MATERIAL_HEADER
#define MATERIAL_HEADER

#include <memory>
#include "Shader_Program.hpp"

class Material
{
private:
    std::shared_ptr<Shader_Program> shader_program;

public:
    Material(std::shared_ptr<Shader_Program> shader);
    void use();
    std::shared_ptr<Shader_Program> get_shader() const { return shader_program; }
};

#endif