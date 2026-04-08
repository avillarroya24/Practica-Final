#include "Camera.hpp"
#include <cmath>

// =======================
// CONSTRUCTOR
// =======================
Camera::Camera()
{
    posX = 0.0f;
    posY = 2.0f;
    posZ = 5.0f;

    rotX = 0.0f;
    rotY = 3.1416f;

    speed = 5.0f;
    sensitivity = 0.002f;
}

// =======================
// DIRECCIÓN
// =======================
void Camera::getDirection(float& dirX, float& dirY, float& dirZ) const
{
    dirX = cos(rotX) * cos(rotY);
    dirY = sin(rotX);
    dirZ = cos(rotX) * sin(rotY);
}

// =======================
// MOVIMIENTO INDIVIDUAL
// =======================
void Camera::moveForward(float deltaTime)
{
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    posX += dirX * speed * deltaTime;
    posY += dirY * speed * deltaTime;
    posZ += dirZ * speed * deltaTime;
}

void Camera::moveBackward(float deltaTime)
{
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    posX -= dirX * speed * deltaTime;
    posY -= dirY * speed * deltaTime;
    posZ -= dirZ * speed * deltaTime;
}

void Camera::moveRight(float deltaTime)
{
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    float rightX = -dirZ;
    float rightZ = dirX;

    posX += rightX * speed * deltaTime;
    posZ += rightZ * speed * deltaTime;
}

void Camera::moveLeft(float deltaTime)
{
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    float rightX = -dirZ;
    float rightZ = dirX;

    posX -= rightX * speed * deltaTime;
    posZ -= rightZ * speed * deltaTime;
}

void Camera::moveUp(float deltaTime)
{
    posY += speed * deltaTime;
}

void Camera::moveDown(float deltaTime)
{
    posY -= speed * deltaTime;
}

// =======================
// CONTROL TECLADO (PRO)
// =======================
void Camera::handleKeyboard(bool forward, bool backward, bool left, bool right, bool up, bool down, float deltaTime)
{
    float dirX, dirY, dirZ;
    getDirection(dirX, dirY, dirZ);

    // Normalizar dirección
    float len = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    dirX /= len;
    dirY /= len;
    dirZ /= len;

    float rightX = -dirZ;
    float rightZ = dirX;

    float moveX = 0.0f;
    float moveY = 0.0f;
    float moveZ = 0.0f;

    if (forward) {
        moveX += dirX;
        moveY += dirY;
        moveZ += dirZ;
    }
    if (backward) {
        moveX -= dirX;
        moveY -= dirY;
        moveZ -= dirZ;
    }
    if (right) {
        moveX += rightX;
        moveZ += rightZ;
    }
    if (left) {
        moveX -= rightX;
        moveZ -= rightZ;
    }
    if (up) moveY += 1.0f;
    if (down) moveY -= 1.0f;

    // Normalizar movimiento final
    float moveLen = sqrt(moveX * moveX + moveY * moveY + moveZ * moveZ);

    if (moveLen > 0.0f)
    {
        moveX /= moveLen;
        moveY /= moveLen;
        moveZ /= moveLen;

        posX += moveX * speed * deltaTime;
        posY += moveY * speed * deltaTime;
        posZ += moveZ * speed * deltaTime;
    }
}

// =======================
// ROTACIÓN
// =======================
void Camera::rotate(float deltaX, float deltaY)
{
    rotY += deltaX * sensitivity;
    rotX -= deltaY * sensitivity;

    // Limitar pitch
    if (rotX > 1.5f) rotX = 1.5f;
    if (rotX < -1.5f) rotX = -1.5f;
}

// =======================
// RATÓN COMPLETO
// =======================
void Camera::handleMouse(float dx, float dy, float dt)
{
    rotate(dx, dy);
}

// =======================
// SETTERS
// =======================
void Camera::setSpeed(float s)
{
    speed = s;
}

void Camera::setSensitivity(float s)
{
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