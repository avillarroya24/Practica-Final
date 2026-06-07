#include "Light.hpp"
#include <glm.hpp>
#include <glad/gl.h>
#include <string>

/*
    Implementación de la clase Light.
    Sistema de iluminación compatible con shading global.
*/

// ================= CONSTRUCTOR =================

Light::Light(LightType t)
    : type(t),
    position(0.0f, 50.0f, 0.0f),
    direction(glm::normalize(glm::vec3(-0.4f, -1.0f, -0.25f))),
    ambient(0.25f, 0.25f, 0.28f),   // ligeramente más equilibrado
    diffuse(1.0f, 0.95f, 0.85f),
    specular(1.0f, 1.0f, 1.0f),     // specular más limpio para toda la escena
    intensity(1.2f)                 // un poco menos agresivo (más natural)
{
}

// ================= SETTERS =================

void Light::setPosition(const glm::vec3& pos) { position = pos; }
void Light::setDirection(const glm::vec3& dir) { direction = glm::normalize(dir); }
void Light::setAmbient(const glm::vec3& a) { ambient = a; }
void Light::setDiffuse(const glm::vec3& d) { diffuse = d; }
void Light::setSpecular(const glm::vec3& s) { specular = s; }
void Light::setIntensity(float i) { intensity = i; }

// ================= GETTERS =================

glm::vec3 Light::getPosition() const { return position; }
glm::vec3 Light::getDirection() const { return direction; }
glm::vec3 Light::getAmbient() const { return ambient; }
glm::vec3 Light::getDiffuse() const { return diffuse; }
glm::vec3 Light::getSpecular() const { return specular; }
float Light::getIntensity() const { return intensity; }
Light::LightType Light::getType() const { return type; }

// ================= APPLY =================

void Light::apply(GLuint program, int index)
{
    std::string base = "lights[" + std::to_string(index) + "]";

    glm::vec3 pos = (type == DIRECTIONAL) ? glm::vec3(0.0f) : position;

    auto setVec3 = [&](const std::string& name, const glm::vec3& v)
        {
            glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &v[0]);
        };

    auto setFloat = [&](const std::string& name, float v)
        {
            glUniform1f(glGetUniformLocation(program, name.c_str()), v);
        };

    auto setInt = [&](const std::string& name, int v)
        {
            glUniform1i(glGetUniformLocation(program, name.c_str()), v);
        };

    // ================= CORE LIGHT DATA =================

    setVec3(base + ".position", pos);
    setVec3(base + ".direction", direction);

    setVec3(base + ".ambient", ambient);
    setVec3(base + ".diffuse", diffuse);
    setVec3(base + ".specular", specular);

    setFloat(base + ".intensity", intensity);
    setInt(base + ".type", type);
}