#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera {
private:
    float posX, posY, posZ;     // Posición
    float rotX, rotY;           // Rotación (pitch, yaw)
    float speed;                // Velocidad de movimiento
    float sensitivity;          // Sensibilidad del ratón

public:
    // Constructor
    Camera();

    // Movimiento
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);

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

#endif