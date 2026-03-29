#include "Camera.hpp"
#include <cmath>

// Constructor
Camera::Camera() {
    posX = 0.0f;
    posY = 0.0f;
    posZ = 0.0f;

    rotX = 0.0f;
    rotY = 0.0f;

    speed = 5.0f;
    sensitivity = 0.1f;
}

// Movimiento hacia delante (DIRECCIÓN REAL DE LA CÁMARA)
void Camera::moveForward(float deltaTime) {
    posX += cos(rotY) * cos(rotX) * speed * deltaTime;
    posY += sin(rotX) * speed * deltaTime;
    posZ += sin(rotY) * cos(rotX) * speed * deltaTime;
}

// Movimiento hacia atrás
void Camera::moveBackward(float deltaTime) {
    posX -= cos(rotY) * cos(rotX) * speed * deltaTime;
    posY -= sin(rotX) * speed * deltaTime;
    posZ -= sin(rotY) * cos(rotX) * speed * deltaTime;
}

// Movimiento izquierda
void Camera::moveLeft(float deltaTime) {
    posX += sin(rotY) * speed * deltaTime;
    posZ -= cos(rotY) * speed * deltaTime;
}

// Movimiento derecha
void Camera::moveRight(float deltaTime) {
    posX -= sin(rotY) * speed * deltaTime;
    posZ += cos(rotY) * speed * deltaTime;
}

// Rotación con ratón
void Camera::rotate(float deltaX, float deltaY) {
    rotY += deltaX * sensitivity;
    rotX += deltaY * sensitivity;

    // Limitar rotación vertical
    if (rotX > 1.5f) rotX = 1.5f;
    if (rotX < -1.5f) rotX = -1.5f;
}

// Setters
void Camera::setSpeed(float s) {
    speed = s;
}

void Camera::setSensitivity(float s) {
    sensitivity = s;
}

// Getters
float Camera::getX() const { return posX; }
float Camera::getY() const { return posY; }
float Camera::getZ() const { return posZ; }

float Camera::getRotX() const { return rotX; }
float Camera::getRotY() const { return rotY; }