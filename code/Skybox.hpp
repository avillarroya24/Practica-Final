#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <string>
#include <glad/gl.h>
#include <glm.hpp>
#include "Camera.hpp"

class Skybox
{
public:
    // ================= CONSTRUCTOR =================
    Skybox(const std::string& texture_path);

    // ================= RENDER =================
    // Ahora usa directamente la cámara (como el código del profe)
    void render(const Camera& camera);

    // ================= DESTRUCTOR =================
    ~Skybox();

private:
    // ================= OPENGL IDS =================
    GLuint vao_id = 0;
    GLuint vbo_id = 0;
    GLuint cubemap_texture = 0;
    GLuint shader_id = 0;

    // ================= GEOMETRÍA =================
    static const GLfloat coordinates[108];

    // ================= TEXTURA =================
    GLuint loadCubemap(const std::string& texture_path);

    // ================= SHADERS =================
    static const char* vertex_shader_code;
    static const char* fragment_shader_code;
};

#endif