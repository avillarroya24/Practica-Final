#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera
{
private:
    // ================= POSICIÓN =================
    glm::vec3 position;

    // ================= ROTACIÓN =================
    float rotX; // pitch
    float rotY; // yaw

    // ================= CONFIG =================
    float speed;
    float sensitivity;

    // ================= MATRICES =================
    glm::mat4 projection_matrix;

public:
    // ================= CONSTRUCTOR =================
    Camera();

    // ================= DIRECCIÓN =================
    void getDirection(float& dirX, float& dirY, float& dirZ) const;

    // ================= MOVIMIENTO =================
    void moveForward(float dt);
    void moveBackward(float dt);
    void moveRight(float dt);
    void moveLeft(float dt);
    void moveUp(float dt);
    void moveDown(float dt);

    // ================= INPUT =================
    void handleMouse(float dx, float dy, float dt);
    void rotate(float dx, float dy);

    // ================= MATRICES (CLAVE SKYBOX) =================
    glm::mat4 get_transform_matrix_inverse() const;
    const glm::mat4& get_projection_matrix() const;

    // ================= SETTERS =================
    void setPosition(float x, float y, float z);
    void setSpeed(float s);
    void setSensitivity(float s);
    void setRatio(float ratio);

    // ================= GETTERS =================
    float getX() const;
    float getY() const;
    float getZ() const;

    float getRotX() const;
    float getRotY() const;
};