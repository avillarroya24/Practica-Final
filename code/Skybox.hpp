#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <string>
#include <glad/gl.h>
#include <glm.hpp>
#include "Camera.hpp"

class Camera;

class Skybox
{
public:

    // Constructor: recibe la ruta base del cubemap
    Skybox(const std::string& path);

    // Render del skybox
    void render(const Camera& camera);

    // Destructor
    ~Skybox();
    Skybox* skybox;

private:

    // ====== CARGA DEL CUBEMAP ======
    GLuint loadCubemap(const std::string& base_path);

    // ====== SHADERS ======
    static const char* vertex_shader_code;
    static const char* fragment_shader_code;

    // ====== GEOMETRÍA DEL CUBO ======
    static const GLfloat coordinates[];

    // ====== IDs OpenGL ======
    GLuint vao_id = 0;
    GLuint vbo_id = 0;
    GLuint shader_id = 0;
    GLuint cubemap_texture = 0;
};

#endif
