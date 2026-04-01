#include "Camera.hpp"
#include <cmath>

Camera::Camera() {
    posX = 0.0f;
    posY = 2.0f; // altura inicial
    posZ = 5.0f;

    rotX = 0.0f;
    rotY = 3.14f; // mirar hacia Z negativa por defecto

    speed = 5.0f;
    sensitivity = 0.002f; // más suave para mouse
}

// Movimiento horizontal (XZ)
void Camera::moveForward(float deltaTime) {
    posX += cos(rotY) * speed * deltaTime;
    posZ += sin(rotY) * speed * deltaTime;
}
void Camera::moveBackward(float deltaTime) {
    posX -= cos(rotY) * speed * deltaTime;
    posZ -= sin(rotY) * speed * deltaTime;
}
void Camera::moveLeft(float deltaTime) {
    posX += sin(rotY) * speed * deltaTime;
    posZ -= cos(rotY) * speed * deltaTime;
}
void Camera::moveRight(float deltaTime) {
    posX -= sin(rotY) * speed * deltaTime;
    posZ += cos(rotY) * speed * deltaTime;
}

// Movimiento vertical
void Camera::moveUp(float deltaTime) { posY += speed * deltaTime; }
void Camera::moveDown(float deltaTime) { posY -= speed * deltaTime; }

// Rotación con ratón
void Camera::rotate(float deltaX, float deltaY) {
    rotY += deltaX * sensitivity;
    rotX += deltaY * sensitivity;

    // Limitar pitch
    if (rotX > 1.5f) rotX = 1.5f;
    if (rotX < -1.5f) rotX = -1.5f;
}

// Setters
void Camera::setSpeed(float s) { speed = s; }
void Camera::setSensitivity(float s) { sensitivity = s; }

// Getters
float Camera::getX() const { return posX; }
float Camera::getY() const { return posY; }
float Camera::getZ() const { return posZ; }

float Camera::getRotX() const { return rotX; }
float Camera::getRotY() const { return rotY; }