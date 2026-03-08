#include "Camera.hpp"
#include <cmath>

namespace udit
{
    Camera::Camera(float fov, float near_z, float far_z, float ratio)
        : fov(fov), near_z(near_z), far_z(far_z), ratio(ratio), yaw(0.f), pitch(0.f)
    {
        location = glm::vec4(0.f, 0.f, 0.f, 1.f);
        target = glm::vec4(0.f, 0.f, -1.f, 1.f);
        update_projection();
    }

    glm::vec4 Camera::get_location() const { return location; }
    glm::vec4 Camera::get_target()   const { return target; }
    glm::mat4 Camera::get_projection_matrix() const { return projection; }
    glm::mat4 Camera::get_view_matrix() const
    {
        return glm::lookAt(glm::vec3(location), glm::vec3(target), glm::vec3(0.f, 1.f, 0.f));
    }

    void Camera::set_location(float x, float y, float z)
    {
        location = glm::vec4(x, y, z, 1.f);
        update_target();
    }

    void Camera::set_target(float x, float y, float z)
    {
        target = glm::vec4(x, y, z, 1.f);
    }

    void Camera::move(const glm::vec3& translation)
    {
        location += glm::vec4(translation, 0.f);
        target += glm::vec4(translation, 0.f);
    }

    void Camera::rotate(float yaw_offset, float pitch_offset)
    {
        yaw += yaw_offset;
        pitch += pitch_offset;

        // Limitar pitch
        if (pitch > 1.5f) pitch = 1.5f;
        if (pitch < -1.5f) pitch = -1.5f;

        update_target();
    }

    void Camera::reset(float fov, float near_z, float far_z, float ratio)
    {
        this->fov = fov;
        this->near_z = near_z;
        this->far_z = far_z;
        this->ratio = ratio;
        location = glm::vec4(0.f, 0.f, 0.f, 1.f);
        yaw = pitch = 0.f;
        update_projection();
        update_target();
    }

    void Camera::update_projection()
    {
        projection = glm::perspective(glm::radians(fov), ratio, near_z, far_z);
    }

    void Camera::update_target()
    {
        glm::vec3 dir;
        dir.x = cos(pitch) * sin(yaw);
        dir.y = sin(pitch);
        dir.z = -cos(pitch) * cos(yaw);
        target = location + glm::vec4(dir, 0.f);
    }
}