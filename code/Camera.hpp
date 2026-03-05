#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    float yaw, pitch;

    Camera(glm::vec3 pos);

    glm::mat4 getViewMatrix();
};

#endif