#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera
{
private:

    // ================= POSICIÓN =================
    glm::vec3 position;

    // ================= ROTACIÓN FPS =================
    float rotX; // pitch
    float rotY; // yawsd

    // ================= MOVIMIENTO =================
    float speed;
    float sensitivity;

    // ================= PROYECCIÓN =================
    float fov;
    float near_z;
    float far_z;
    float ratio;

    glm::mat4 projection_matrix;

private:
    void updateProjection();

public:

    // ================= CONSTRUCTOR =================
    Camera();

    // ================= DIRECCIÓN =================
    void getDirection(float& x, float& y, float& z) const;

    // ================= MOVIMIENTO =================
    void moveForward(float dt);
    void moveBackward(float dt);
    void moveRight(float dt);
    void moveLeft(float dt);
    void moveUp(float dt);
    void moveDown(float dt);

    // ================= ROTACIÓN =================
    void rotate(float dx, float dy);

    // ================= MATRICES =================
    glm::mat4 get_view_matrix() const;
    const glm::mat4& get_projection_matrix() const;

    // ================= SETTERS =================
    void setPosition(float x, float y, float z);
    void setSpeed(float s);
    void setSensitivity(float s);
    void setRatio(float r);

    // ================= GETTERS =================
    float getX() const;
    float getY() const;
    float getZ() const;

    float getRotX() const;
    float getRotY() const;
};