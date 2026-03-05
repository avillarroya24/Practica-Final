#include "Camera.hpp"
#include <glm.hpp>                  // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp> // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>         // value_ptr

namespace udit
{
    // --- MOVE GENERAL ---
    void Camera::move(const glm::vec3& translation)
    {
        location += glm::vec4(translation, 1.f);
        target += glm::vec4(translation, 1.f);
    }

    void Camera::rotate(const glm::mat4& rotation)
    {
        target = location + rotation * (target - location);
    }

    // --- MOVIMIENTO CONTROLABLE ---
    void Camera::move_forward(float distance)
    {
        glm::vec3 dir = glm::normalize(glm::vec3(target - location));
        move(dir * distance);
    }

    void Camera::move_backward(float distance)
    {
        move_forward(-distance);
    }

    void Camera::move_right(float distance)
    {
        glm::vec3 forward = glm::normalize(glm::vec3(target - location));
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.f, 1.f, 0.f)));
        move(right * distance);
    }

    void Camera::move_left(float distance)
    {
        move_right(-distance);
    }

    void Camera::move_up(float distance)
    {
        move(glm::vec3(0.f, distance, 0.f));
    }

    void Camera::move_down(float distance)
    {
        move(glm::vec3(0.f, -distance, 0.f));
    }

    // --- PROYECCION ---
    void Camera::calculate_projection_matrix()
    {
        projection_matrix = glm::perspective(glm::radians(fov), ratio, near_z, far_z);
    }

    // --- MATRIZ INVERSA (LOOKAT) ---
    glm::mat4 Camera::get_transform_matrix_inverse() const
    {
        return glm::lookAt(
            glm::vec3(location.x, location.y, location.z),
            glm::vec3(target.x, target.y, target.z),
            glm::vec3(0.f, 1.f, 0.f)
        );
    }
}
