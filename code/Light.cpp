#include "Light.hpp"
#include <glm.hpp>
#include <glad/gl.h>

// ================= CONSTRUCTOR =================
Light::Light(LightType t)
    : type(t),
    position(0.0f, 5.0f, 0.0f),

    // Dirección tipo sol (más natural)
    direction(glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f))),

    // Luz más homogénea en toda la escena
    ambient(0.35f, 0.35f, 0.35f),
    diffuse(0.9f, 0.9f, 0.9f),
    specular(0.6f, 0.6f, 0.6f),

    intensity(1.0f)
{
}

// ================= SETTERS =================
void Light::setPosition(const glm::vec3& pos)
{
    position = pos;
}

void Light::setDirection(const glm::vec3& dir)
{
    // IMPORTANTE: siempre normalizada
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

    // Si es luz direccional no depende de posición
    if (type == DIRECTIONAL)
    {
        glm::vec3 fakePos(0.0f, 0.0f, 0.0f);
        glUniform3fv(glGetUniformLocation(program, (base + ".position").c_str()), 1, &fakePos[0]);
    }
    else
    {
        glUniform3fv(glGetUniformLocation(program, (base + ".position").c_str()), 1, &position[0]);
    }

    glUniform3fv(glGetUniformLocation(program, (base + ".direction").c_str()), 1, &direction[0]);

    glUniform3fv(glGetUniformLocation(program, (base + ".ambient").c_str()), 1, &ambient[0]);
    glUniform3fv(glGetUniformLocation(program, (base + ".diffuse").c_str()), 1, &diffuse[0]);
    glUniform3fv(glGetUniformLocation(program, (base + ".specular").c_str()), 1, &specular[0]);

    glUniform1f(glGetUniformLocation(program, (base + ".intensity").c_str()), intensity);
    glUniform1i(glGetUniformLocation(program, (base + ".type").c_str()), type);
}