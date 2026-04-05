#include "Camera.hpp"
#include <cmath>

// Constructor
Camera::Camera() {
    posX = 0.0f;
    posY = 2.0f;
    posZ = 5.0f;

    rotX = 0.0f;
    rotY = 3.1416f;

    speed = 5.0f;
    sensitivity = 0.002f;
}

// =======================
// DIRECCIÓN DE LA CÁMARA
// =======================
void Camera::getDirection(float& dirX, float& dirY, float& dirZ) const {
    dirX = cos(rotX) * cos(rotY);
    dirY = sin(rotX);
    dirZ = cos(rotX) * sin(rotY);
}

// =======================
// MOVIMIENTO
// =======================
void Camera::moveForward(float deltaTime) {
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    posX += dirX * speed * deltaTime;
    posY += dirY * speed * deltaTime;
    posZ += dirZ * speed * deltaTime;
}

void Camera::moveBackward(float deltaTime) {
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    posX -= dirX * speed * deltaTime;
    posY -= dirY * speed * deltaTime;
    posZ -= dirZ * speed * deltaTime;
}

void Camera::moveRight(float deltaTime) {
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    float rightX = -dirZ;
    float rightZ = dirX;

    posX += rightX * speed * deltaTime;
    posZ += rightZ * speed * deltaTime;
}

void Camera::moveLeft(float deltaTime) {
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    float rightX = -dirZ;
    float rightZ = dirX;

    posX -= rightX * speed * deltaTime;
    posZ -= rightZ * speed * deltaTime;
}

// Movimiento vertical
void Camera::moveUp(float deltaTime) {
    posY += speed * deltaTime;
}

void Camera::moveDown(float deltaTime) {
    posY -= speed * deltaTime;
}

// =======================
// ROTACIÓN (RATÓN)
// =======================
void Camera::rotate(float deltaX, float deltaY) {
    rotY += deltaX * sensitivity;
    rotX -= deltaY * sensitivity;

    // Limitar pitch
    if (rotX > 1.5f) rotX = 1.5f;
    if (rotX < -1.5f) rotX = -1.5f;
}

// =======================
// CONTROL COMPLETO CON RATÓN
// =======================
void Camera::handleMouse(float dx, float dy, float dt)
{
    // 1. ROTACIÓN (mirar)
    rotate(dx, dy);

    // 2. MOVIMIENTO PROPORCIONAL
    float force = 0.01f;
    float deadZone = 1.0f;

    // Adelante / atrás
    if (fabs(dy) > deadZone)
    {
        float amount = fabs(dy) * force;

        if (dy < 0)
            moveForward(dt * amount);
        else
            moveBackward(dt * amount);
    }

    // Strafe izquierda / derecha
    if (fabs(dx) > deadZone)
    {
        float amount = fabs(dx) * force;

        if (dx > 0)
            moveRight(dt * amount);
        else
            moveLeft(dt * amount);
    }
}

// =======================
// SETTERS
// =======================
void Camera::setSpeed(float s) {
    speed = s;
}

void Camera::setSensitivity(float s) {
    sensitivity = s;
}

// =======================
// GETTERS
// =======================
float Camera::getX() const { return posX; }
float Camera::getY() const { return posY; }
float Camera::getZ() const { return posZ; }

float Camera::getRotX() const { return rotX; }
float Camera::getRotY() const { return rotY; }