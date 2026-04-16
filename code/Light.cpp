#include "Light.hpp"
#include <glm.hpp>
#include <glad/gl.h>
#include <string>

// ================= CONSTRUCTOR =================
Light::Light(LightType t)
    : type(t),
    position(0.0f, 50.0f, 0.0f),
    // Dirección tipo sol, ligeramente inclinada
    direction(glm::normalize(glm::vec3(-0.4f, -1.0f, -0.25f))),

    // Luz ambiental suave y ligeramente fría
    ambient(0.22f, 0.22f, 0.25f),

    // Luz difusa cálida tipo sol
    diffuse(1.0f, 0.95f, 0.85f),

    // Especular limpio y brillante
    specular(1.0f, 0.95f, 0.9f),

    // Intensidad global
    intensity(1.4f)
{
}

// ================= SETTERS =================
void Light::setPosition(const glm::vec3& pos)
{
    position = pos;
}

void Light::setDirection(const glm::vec3& dir)
{
    direction = glm::normalize(dir);
}

void Light::setAmbient(const glm::vec3& a)
{
    ambient = a;
}

void Light::setDiffuse(const glm::vec3& d)
{
    diffuse = d;
}

void Light::setSpecular(const glm::vec3& s)
{
    specular = s;
}

void Light::setIntensity(float i)
{
    intensity = i;
}

// ================= GETTERS =================
glm::vec3 Light::getPosition() const
{
    return position;
}

glm::vec3 Light::getDirection() const
{
    return direction;
}

glm::vec3 Light::getAmbient() const
{
    return ambient;
}

glm::vec3 Light::getDiffuse() const
{
    return diffuse;
}

glm::vec3 Light::getSpecular() const
{
    return specular;
}

float Light::getIntensity() const
{
    return intensity;
}

Light::LightType Light::getType() const
{
    return type;
}

// ================= APPLY =================
void Light::apply(GLuint program, int index)
{
    std::string base = "lights[" + std::to_string(index) + "]";

    // Para direccional, la posición no importa: se usa direction
    glm::vec3 posToSend = (type == DIRECTIONAL)
        ? glm::vec3(0.0f)
        : position;

    glUniform3fv(glGetUniformLocation(program, (base + ".position").c_str()), 1, &posToSend[0]);
    glUniform3fv(glGetUniformLocation(program, (base + ".direction").c_str()), 1, &direction[0]);

    glUniform3fv(glGetUniformLocation(program, (base + ".ambient").c_str()), 1, &ambient[0]);
    glUniform3fv(glGetUniformLocation(program, (base + ".diffuse").c_str()), 1, &diffuse[0]);
    glUniform3fv(glGetUniformLocation(program, (base + ".specular").c_str()), 1, &specular[0]);

    glUniform1f(glGetUniformLocation(program, (base + ".intensity").c_str()), intensity);
    glUniform1i(glGetUniformLocation(program, (base + ".type").c_str()), type);
}
