#include "Skybox.hpp"
#include <iostream>
#include <SOIL2.h>
#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

// ================= CUBO =================
const GLfloat Skybox::coordinates[] = {
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f
};

// ================= SHADERS =================

// Vertex shader
const char* Skybox::vertex_shader_code =
"#version 330 core\n"
"layout(location = 0) in vec3 vertex_coordinates;\n"
"out vec3 tex_coords;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main()\n"
"{\n"
"    tex_coords = vertex_coordinates;\n"
"    vec4 pos = projection * view * vec4(vertex_coordinates, 1.0);\n"
"    gl_Position = pos.xyww;\n"
"}";

// Fragment shader
const char* Skybox::fragment_shader_code =
"#version 330 core\n"
"in vec3 tex_coords;\n"
"out vec4 FragColor;\n"
"uniform samplerCube skybox;\n"
"void main()\n"
"{\n"
"    FragColor = texture(skybox, tex_coords);\n"
"}";

// ================= CUBEMAP =================
GLuint Skybox::loadCubemap(const std::string& texture_path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Cargar la misma imagen en las 6 caras
    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, channels;
        unsigned char* data = SOIL_load_image(texture_path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

        if (!data)
        {
            std::cerr << "Error cargando textura: " << texture_path << std::endl;
            glDeleteTextures(1, &textureID);
            return 0;
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );

        SOIL_free_image_data(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// ================= CONSTRUCTOR =================
Skybox::Skybox(const std::string& texture_path)
{
    cubemap_texture = loadCubemap(texture_path);

    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_id);

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
}

// ================= RENDER =================
void Skybox::render(const glm::mat4& view,
    const glm::mat4& projection,
    GLuint shader)
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader);

    glm::mat4 view_no_translation = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_no_translation));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}

// ================= DESTRUCTOR =================
Skybox::~Skybox()
{
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteTextures(1, &cubemap_texture);
}