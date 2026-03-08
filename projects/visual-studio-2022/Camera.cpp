#include "Camera.hpp"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    pos = { 0, 3, 10 };
    yaw = -90;
    pitch = -15;
    speed = 8;
    sens = 0.1f;
}

glm::mat4 Camera::getView() const
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::lookAt(pos, pos + glm::normalize(front), { 0,1,0 });
}

void Camera::moveForward(float dt) { pos += dt * speed * glm::normalize(glm::vec3(cos(glm::radians(yaw)), 0, sin(glm::radians(yaw)))); }
void Camera::moveBackward(float dt) { pos -= dt * speed * glm::normalize(glm::vec3(cos(glm::radians(yaw)), 0, sin(glm::radians(yaw)))); }
void Camera::moveLeft(float dt) { pos -= glm::normalize(glm::cross(glm::normalize(glm::vec3(cos(glm::radians(yaw)), 0, sin(glm::radians(yaw)))), { 0,1,0 })) * dt * speed; }
void Camera::moveRight(float dt) { pos += glm::normalize(glm::cross(glm::normalize(glm::vec3(cos(glm::radians(yaw)), 0, sin(glm::radians(yaw)))), { 0,1,0 })) * dt * speed; }

void Camera::mouse(float dx, float dy)
{
    yaw += dx * sens;
    pitch += dy * sens;
    if (pitch > 89) pitch = 89;
    if (pitch < -89) pitch = -89;
}
