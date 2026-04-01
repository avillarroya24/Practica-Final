#pragma once
#include <glm.hpp>
#include <string>
#include <glad/gl.h>

class Light
{
public:
    enum LightType
    {
        DIRECTIONAL,
        POINT,
        SPOT
    };

private:
    LightType type;

    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float intensity;

public:
    // Constructor
    Light(LightType t = POINT);

    // Setters
    void setPosition(const glm::vec3& pos);
    void setDirection(const glm::vec3& dir);
    void setAmbient(const glm::vec3& a);
    void setDiffuse(const glm::vec3& d);
    void setSpecular(const glm::vec3& s);
    void setIntensity(float i);

    // Getters
    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;
    glm::vec3 getAmbient() const;
    glm::vec3 getDiffuse() const;
    glm::vec3 getSpecular() const;
    float getIntensity() const;
    LightType getType() const;

    // Aplicar al shader
    void apply(GLuint program, int index = 0);
};