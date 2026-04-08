#pragma once
#include "Node.hpp"

class Camera : public Node
{
private:
    // Posición
    float posX, posY, posZ;

    // Rotación (pitch y yaw)
    float rotX, rotY;

    // Configuración
    float speed;
    float sensitivity;

public:
    // Constructor
    Camera();

    // Dirección
    void getDirection(float& dirX, float& dirY, float& dirZ) const;

    // Movimiento individual
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveRight(float deltaTime);
    void moveLeft(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);

    // Control completo
    void handleKeyboard(bool forward, bool backward, bool left, bool right, bool up, bool down, float deltaTime);
    void handleMouse(float dx, float dy, float dt);

    // Rotación
    void rotate(float deltaX, float deltaY);

    // Setters
    void setSpeed(float s);
    void setSensitivity(float s);

    // Getters
    float getX() const;
    float getY() const;
    float getZ() const;

    float getRotX() const;
    float getRotY() const;
};
