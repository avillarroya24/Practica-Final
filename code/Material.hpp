#pragma once

#include <memory>

#include "Shader_Program.hpp"

using namespace std;

class Material
{
public:

    shared_ptr<Shader_Program> shader_program;

public:

    Material();

    void use();
};