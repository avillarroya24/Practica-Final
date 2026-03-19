#ifndef SKYBOX_HEADER
#define SKYBOX_HEADER

#include <string>
#include <glad/gl.h>
#include "Texture_Cube.hpp"
#include "Shader_Program.hpp"
#include "Material.hpp"

class Skybox
{
private:
    GLuint vao_id = 0;
    GLuint vbo_id = 0;
    GLuint shader_program_id = 0;

    Material material;

    GLuint model_view_id = 0;
    GLuint projection_id = 0;

    udit::Texture_Cube cubemap;

    static const GLfloat coordinates[];
    static const char* vertex_shader_code;
    static const char* fragment_shader_code;

    GLuint compile_shaders();

public:
public:
    Skybox(const std::string& texture_base_path, std::shared_ptr<Shader_Program> shader);
    void render(const float* view_matrix, const float* projection_matrix);
    ~Skybox();
};

#endif