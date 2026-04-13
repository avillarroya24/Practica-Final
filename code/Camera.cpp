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

    fov = 60.0f;
    near_z = 0.1f;
    far_z = 5000.0f;
    ratio = 16.0f / 9.0f;

    updateProjection();
}

// =======================
// PROJECTION
// =======================
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
// DIRECCIÓN (NORMALIZADA)
// =======================
void Camera::getDirection(float& x, float& y, float& z) const
{
    x = cos(rotX) * cos(rotY);
    y = sin(rotX);
    z = cos(rotX) * sin(rotY);
}

// =======================
// MOVIMIENTO RELATIVO
// =======================
void Camera::moveForward(float dt)
{
    float x, y, z;
    getDirection(x, y, z);

    glm::vec3 forward = glm::normalize(glm::vec3(x, y, z));
    position += forward * speed * dt;
}

void Camera::moveBackward(float dt)
{
    float x, y, z;
    getDirection(x, y, z);

    glm::vec3 forward = glm::normalize(glm::vec3(x, y, z));
    position -= forward * speed * dt;
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

void Camera::moveUp(float dt)
{
    position.y += speed * dt;
}

void Camera::moveDown(float dt)
{
    position.y -= speed * dt;
}

// =======================
// ROTACIÓN LIBRE (FPS + SMOOTH)
// =======================
void Camera::rotate(float dx, float dy)
{
    rotY += dx * sensitivity;
    rotX -= dy * sensitivity;

    //CLAMP SUAVE (evita flip completo)
    const float limit = 1.55f;

    if (rotX > limit) rotX = limit;
    if (rotX < -limit) rotX = -limit;
}

// =======================
// VIEW MATRIX
// =======================
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
// PROJECTION
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

void Camera::setRatio(float r)
{
    ratio = r;
    updateProjection();
}

// =======================
// GETTERS
// =======================
float Camera::getX() const { return position.x; }
float Camera::getY() const { return position.y; }
float Camera::getZ() const { return position.z; }

float Camera::getRotX() const { return rotX; }
float Camera::getRotY() const { return rotY; }