#include "Light.hpp"
#include <glm.hpp>
#include <glad/gl.h>
#include <string>

/*
    Implementación de la clase Light.
 
 * Este archivo define el comportamiento de las fuentes de luz en la escena,
   incluyendo sus propiedades (posición, dirección, color, intensidad) y
   su envío al shader mediante uniforms de OpenGL.
    
*/

// ================= CONSTRUCTOR =================

/*

    Constructor de la luz.
 
 * Inicializa una fuente de luz con valores por defecto según su tipo.
 
 * @param t Tipo de luz (direccional, puntual, etc.).
*/
Light::Light(LightType t)
    : type(t),
    position(0.0f, 50.0f, 0.0f),
    direction(glm::normalize(glm::vec3(-0.4f, -1.0f, -0.25f))),
    ambient(0.22f, 0.22f, 0.25f),
    diffuse(1.0f, 0.95f, 0.85f),
    specular(1.0f, 0.95f, 0.9f),
    intensity(1.4f)
{
}

// ================= SETTERS =================

/*

    Establece la luz.

 * @param pos Nueva posición.
*/
void Light::setPosition(const glm::vec3& pos) { position = pos; } //Establece la posicion de la luz
void Light::setDirection(const glm::vec3& dir) { direction = glm::normalize(dir); } //Establece la direccion
void Light::setAmbient(const glm::vec3& a) { ambient = a; } //Establece el componente ambiental
void Light::setDiffuse(const glm::vec3& d) { diffuse = d; } //Establece el componente difuso
void Light::setSpecular(const glm::vec3& s) { specular = s; } //Establece el componente especular
void Light::setIntensity(float i) { intensity = i; } //Establece la intensidad de la luz

// ================= GETTERS =================
glm::vec3 Light::getPosition() const { return position; } //Devuelve la posicion de la luz
glm::vec3 Light::getDirection() const { return direction; } //Devuelve la direccion
glm::vec3 Light::getAmbient() const { return ambient; } //Devuelve el componente ambiental
glm::vec3 Light::getDiffuse() const { return diffuse; } //Devueve el componente difuso
glm::vec3 Light::getSpecular() const { return specular; } //Devuelve el componente especular
float Light::getIntensity() const { return intensity; } //Devuelve la intensidad
Light::LightType Light::getType() const { return type; } //Devuelve el tipo de luz

// ================= APPLY =================

/*

    Aplica la luz al shader.

 * Envía todos los parámetros de la luz al programa de shaders como uniforms,
   permitiendo su uso en los cálculos de iluminación en GPU.

 * @param program ID del programa de shaders activo.
 * @param index Índice de la luz en el array de luces del shader.

*/

void Light::apply(GLuint program, int index)
{
    std::string base = "lights[" + std::to_string(index) + "]";

    glm::vec3 pos = (type == DIRECTIONAL) ? glm::vec3(0.0f) : position; //Si es direccional, no usa posicion

    auto setVec3 = [&](const std::string& name, const glm::vec3& v) //Funciones auxiliares para enviar datos al shader
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

    setVec3(base + ".position", pos); //Envio de parametros al shader
    setVec3(base + ".direction", direction);
    setVec3(base + ".ambient", ambient);
    setVec3(base + ".diffuse", diffuse);
    setVec3(base + ".specular", specular);

    setFloat(base + ".intensity", intensity);
    setInt(base + ".type", type);
}