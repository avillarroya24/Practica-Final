#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <string>
#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

class Skybox
{
private:
    GLuint vao_id;
    GLuint vbo_id;
    GLuint cubemap_texture;

    static const GLfloat coordinates[108]; // 36 vértices * 3 componentes

    // Carga el cubemap usando la misma imagen en las 6 caras
    GLuint loadCubemap(const std::string& texture_path);

public:
    Skybox(const std::string& texture_path);
    ~Skybox();

    void render(const glm::mat4& view,
        const glm::mat4& projection,
        GLuint shader);

    // Shaders para mostrar la textura del cubemap
    static const char* vertex_shader_code;
    static const char* fragment_shader_code;
};

#endif