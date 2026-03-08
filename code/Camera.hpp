#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace udit
{
    class Camera
    {
    public:
        Camera(float fov = 60.f, float near_z = 0.1f, float far_z = 1000.f, float ratio = 1.f);

        // GETTERS
        glm::vec4 get_location() const;
        glm::vec4 get_target() const;
        glm::mat4 get_projection_matrix() const;
        glm::mat4 get_view_matrix() const;

        // SETTERS
        void set_location(float x, float y, float z);
        void set_target(float x, float y, float z);

        // MOVIMIENTO
        void move(const glm::vec3& translation);
        void rotate(float yaw_offset, float pitch_offset);

        // RESETEO
        void reset(float fov, float near_z, float far_z, float ratio);

    private:
        void update_projection();
        void update_target();

        glm::vec4 location;
        glm::vec4 target;
        glm::mat4 projection;

        float fov;
        float near_z;
        float far_z;
        float ratio;

        float yaw;   // rotación horizontal
        float pitch; // rotación vertical
    };
}