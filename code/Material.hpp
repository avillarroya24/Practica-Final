#pragma once
#include <memory>

class Shader_Program;

class Material
{
public:

    std::shared_ptr<Shader_Program> shader_program;

    void use();
};
