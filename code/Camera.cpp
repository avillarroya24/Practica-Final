#include "Camera.hpp"
#include <cmath>
/* Este archivo contiene la implementación de los métodos de la clase Camera,
* encargada de gestionar la posición, orientación y matrices de vista y proyección
* de una cámara 3D.*/

// =======================
// CONSTRUCTOR
// =======================

/* Inicializa la posición, rotación, velocidad y parámetros de proyección
* de la cámara.También calcula la matriz de proyección inicial.
*/
Camera::Camera()
    : position(0.0f, 2.0f, 5.0f),
    rotX(0.0f),
    rotY(3.1416f),
    speed(5.0f),
    sensitivity(0.002f),
    fov(60.0f),
    near_z(0.1f),
    far_z(5000.0f),
    ratio(16.0f / 9.0f)
{
    updateProjection();
}

// =======================
// PROJECTION
// =======================

/*Calcula la matriz de proyección en perspectiva en función del
* campo de visión, ratio de aspecto y planos de recorte.
*/
void Camera::updateProjection()
{
    projection_matrix = glm::perspective(
        glm::radians(fov),
        ratio,
        near_z,
        far_z
    );
}

// =======================
// DIRECCIÓN
// =======================

/* 
    Calcula la dirección hacia donde mira la cámara.
 * 
 * @param x Componente X de la dirección.
 * @param y Componente Y de la dirección.
 * @param z Componente Z de la dirección.

*/

void Camera::getDirection(float& x, float& y, float& z) const
{
    x = cos(rotX) * cos(rotY);
    y = sin(rotX);
    z = cos(rotX) * sin(rotY);
}

// =======================
// MOVIMIENTO
// =======================

/*
    Mueve la cámara en diferentes direcciones (arriba, abajo, hacia adelante, hacia detras...)
 * @param dt Tiempo delta (tiempo entre frames).
*/
void Camera::moveForward(float dt)
{
    float x, y, z;
    getDirection(x, y, z);
    position += glm::normalize(glm::vec3(x, y, z)) * speed * dt;
}

void Camera::moveBackward(float dt)
{
    float x, y, z;
    getDirection(x, y, z);
    position -= glm::normalize(glm::vec3(x, y, z)) * speed * dt;
}

void Camera::moveRight(float dt)
{
    float x, y, z;
    getDirection(x, y, z);

    glm::vec3 forward = glm::normalize(glm::vec3(x, y, z));
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

    position += right * speed * dt;
}

void Camera::moveLeft(float dt)
{
    float x, y, z;
    getDirection(x, y, z);

    glm::vec3 forward = glm::normalize(glm::vec3(x, y, z));
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

    position -= right * speed * dt;
}

void Camera::moveUp(float dt) { position.y += speed * dt; }
void Camera::moveDown(float dt) { position.y -= speed * dt; }

// =======================
// ROTACIÓN
// =======================

/*
    Rota la cámara según el movimiento del ratón.
 * 
 * @param dx Movimiento en el eje X.
 * @param dy Movimiento en el eje Y.

*/
void Camera::rotate(float dx, float dy)
{
    rotY += dx * sensitivity;
    rotX -= dy * sensitivity;

    const float TWO_PI = 6.28318f;
    if (rotY > TWO_PI) rotY -= TWO_PI;
    if (rotY < 0.0f)   rotY += TWO_PI;

    const float limit = 1.55f;
    rotX = glm::clamp(rotX, -limit, limit);
}

// =======================
// VIEW MATRIX
// =======================

/*
    Obtiene la matriz de vista.

 * @return Matriz de vista calculada con glm::lookAt.

*/
glm::mat4 Camera::get_view_matrix() const
{
    float x, y, z;
    getDirection(x, y, z);

    glm::vec3 front = glm::normalize(glm::vec3(x, y, z));

    return glm::lookAt(
        position,
        position + front,
        glm::vec3(0, 1, 0)
    );
}

// =======================
// PROJECTION GETTER
// =======================

/*
    Devuelve la matriz de proyección.

 * @return Referencia constante a la matriz de proyección.

*/
const glm::mat4& Camera::get_projection_matrix() const
{
    return projection_matrix;
}

// =======================
// SETTERS
// =======================

//Establece la posicion de la camara

void Camera::setPosition(float x, float y, float z)
{
    position = { x, y, z };
}

//Establece la velocidad del movimiento
void Camera::setSpeed(float s) { speed = s; }

//Establece la sensibilidad del raton
void Camera::setSensitivity(float s) { sensitivity = s; }

//Establece el ratio de aspecto y actualiza la proyeccion
void Camera::setRatio(float r)
{
    ratio = r;
    updateProjection();
}

// =======================
// GETTERS
// =======================

/*

    Devuelves las coordenadas de la camara (X,Y,Z)
*/
float Camera::getX() const { return position.x; }
float Camera::getY() const { return position.y; }
float Camera::getZ() const { return position.z; }

float Camera::getRotX() const { return rotX; }
float Camera::getRotY() const { return rotY; }