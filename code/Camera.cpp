#include "Camera.hpp"
#include <cmath>

// =======================
// CONSTRUCTOR
// =======================
Camera::Camera()
{
    position = glm::vec3(0.0f, 2.0f, 5.0f);

    rotX = 0.0f;
    rotY = 3.1416f;

    speed = 5.0f;
    sensitivity = 0.002f;

    projection_matrix = glm::perspective(
        glm::radians(60.0f),
        1024.0f / 576.0f,
        0.1f,
        5000.0f
    );
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
// MOVIMIENTO
// =======================
void Camera::moveForward(float dt)
{
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    position.x += dx * speed * dt;
    position.y += dy * speed * dt;
    position.z += dz * speed * dt;
}

void Camera::moveBackward(float dt)
{
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    position.x -= dx * speed * dt;
    position.y -= dy * speed * dt;
    position.z -= dz * speed * dt;
}

void Camera::moveRight(float dt)
{
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    glm::vec3 forward(dx, dy, dz);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

    position += right * speed * dt;
}

void Camera::moveLeft(float dt)
{
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    glm::vec3 forward(dx, dy, dz);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

    position -= right * speed * dt;
}

void Camera::moveUp(float dt)
{
    position.y += speed * dt;
}

void Camera::moveDown(float dt)
{
    position.y -= speed * dt;
}

// =======================
// ROTACIÓN
// =======================
void Camera::rotate(float dx, float dy)
{
    rotY += dx * sensitivity;
    rotX -= dy * sensitivity;

    if (rotX > 1.5f) rotX = 1.5f;
    if (rotX < -1.5f) rotX = -1.5f;
}

// =======================
// INPUT MOUSE
// =======================
void Camera::handleMouse(float dx, float dy, float dt)
{
    rotate(dx, dy);
}

// =======================
// MATRIZ VIEW (SKYBOX)
// =======================
glm::mat4 Camera::get_transform_matrix_inverse() const
{
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    glm::vec3 target = position + glm::vec3(dx, dy, dz);

    return glm::lookAt(
        position,
        target,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

// =======================
// PROJECTION MATRIX
// =======================
const glm::mat4& Camera::get_projection_matrix() const
{
    return projection_matrix;
}

// =======================
// SETTERS
// =======================
void Camera::setPosition(float x, float y, float z)
{
    position = glm::vec3(x, y, z);
}

void Camera::setSpeed(float s)
{
    speed = s;
}

void Camera::setSensitivity(float s)
{
    sensitivity = s;
}

void Camera::setRatio(float ratio)
{
    projection_matrix = glm::perspective(
        glm::radians(60.0f),
        ratio,
        0.1f,
        5000.0f
    );
}

// =======================
// GETTERS
// =======================
float Camera::getX() const { return position.x; }
float Camera::getY() const { return position.y; }
float Camera::getZ() const { return position.z; }

float Camera::getRotX() const { return rotX; }
float Camera::getRotY() const { return rotY; }