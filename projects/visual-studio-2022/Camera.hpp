#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>

class Camera
{
public:
    glm::vec3 pos;
    float yaw, pitch;
    float speed, sens;

    Camera();
    glm::mat4 getView() const;
    void moveForward(float dt);
    void moveBackward(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    void mouse(float dx, float dy);
};
