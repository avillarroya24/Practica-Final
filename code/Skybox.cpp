#include "Skybox.hpp"
#include <iostream>

using namespace std;

const GLfloat Skybox::coordinates[] = {
    -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f
};

const char* Skybox::vertex_shader_code =
"#version 330 core\n"
"layout(location = 0) in vec3 vertex_coordinates;\n"
"uniform mat4 model_view_matrix;\n"
"uniform mat4 projection_matrix;\n"
"out vec3 tex_coords;\n"
"void main() {\n"
"    tex_coords = vertex_coordinates;\n"
"    gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);\n"
"}";

const char* Skybox::fragment_shader_code =
"#version 330 core\n"
"in vec3 tex_coords;\n"
"out vec4 FragColor;\n"
"uniform samplerCube skybox;\n"
"void main() {\n"
"    FragColor = texture(skybox, tex_coords);\n"
"}";

Skybox::Skybox(const std::string& texture_base_path, std::shared_ptr<Shader_Program> shader)
    : cubemap(texture_base_path), material(shader)
{
    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_id);

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);
}

void Skybox::render(const float* view_matrix, const float* projection_matrix)
{
    glDepthMask(GL_FALSE);
    material.use(); // <- ahora usamos Material
    cubemap.bind();

    GLint model_view_id = material.get_shader()->get_uniform_location("model_view_matrix");
    GLint projection_id = material.get_shader()->get_uniform_location("projection_matrix");

    glUniformMatrix4fv(model_view_id, 1, GL_FALSE, view_matrix);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, projection_matrix);

    glBindVertexArray(vao_id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

Skybox::~Skybox()
{
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
}

GLuint Skybox::compile_shaders()
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vs, 1, &vertex_shader_code, nullptr);
    glShaderSource(fs, 1, &fragment_shader_code, nullptr);

    glCompileShader(vs);
    glCompileShader(fs);

    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) { std::cerr << "Error compilando vertex shader\n"; }
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) { std::cerr << "Error compilando fragment shader\n"; }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}
