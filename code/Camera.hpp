#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera {
private:
    float posX, posY, posZ;   // Posición
    float rotX, rotY;         // Pitch (X) y Yaw (Y)
    float speed;              // Velocidad de movimiento
    float sensitivity;        // Sensibilidad del ratón

public:
    Camera();

    // ================= MOVIMIENTO =================
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);

    // ================= ROTACIÓN =================
    void rotate(float deltaX, float deltaY);

    // ================= RATÓN COMPLETO =================
    void handleMouse(float dx, float dy, float dt);

    // ================= DIRECCIÓN =================
    void getDirection(float& dirX, float& dirY, float& dirZ) const;

    // ================= SETTERS =================
    void setSpeed(float s);
    void setSensitivity(float s);

    // ================= GETTERS =================
    float getX() const;
    float getY() const;
    float getZ() const;

    float getRotX() const;
    float getRotY() const;
};

#endif