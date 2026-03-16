#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <array>
#include <cmath>
#include <iostream>

#include <glm.hpp>                          // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp>         // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>                 // value_ptr 

using glm::vec3;
using glm::mat4;

struct Transform
{
    Transform* parent = nullptr;

    vec3 position;
    vec3 rotation; // en grados
    vec3 scale{ 1,1,1 };


    mat4 get_transform_matrix() const;

};

#endif