#include "Skybox.hpp"
#include <iostream>
#include <SOIL2.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// ================= CUBO =================
const GLfloat Skybox::coordinates[] =
{
    -1,-1,-1,  1,-1,-1,  1, 1,-1,  -1,-1,-1,  1, 1,-1, -1, 1,-1,
    -1,-1, 1,  1,-1, 1,  1, 1, 1,  -1,-1, 1,  1, 1, 1, -1, 1, 1,
};

// ================= SHADERS =================
const char* Skybox::vertex_shader_code =
"#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"out vec3 TexCoords;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main() {\n"
"    TexCoords = aPos;\n"
"    vec4 pos = projection * view * vec4(aPos, 1.0);\n"
"    gl_Position = pos.xyww;\n"
"}";

const char* Skybox::fragment_shader_code =
"#version 330 core\n"
"in vec3 TexCoords;\n"
"out vec4 FragColor;\n"
"uniform samplerCube skybox;\n"
"void main() {\n"
"    FragColor = texture(skybox, TexCoords);\n"
"}";

// ================= CUBEMAP =================
GLuint Skybox::loadCubemap(const std::string& base_path)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    for (int i = 0; i < 6; i++)
    {
        int w, h, c;

        unsigned char* data =
            SOIL_load_image(base_path.c_str(), &w, &h, &c, SOIL_LOAD_RGBA);

        if (!data)
        {
            std::cerr << "Error cargando cubemap: " << base_path << std::endl;
            glDeleteTextures(1, &tex);
            return 0;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, data);

        SOIL_free_image_data(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return tex;
}

// ================= CONSTRUCTOR =================
Skybox::Skybox(const std::string& path)
{
    cubemap_texture = loadCubemap(path);

    // -------- SHADER --------
    shader_id = glCreateProgram();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader_code, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader_code, nullptr);
    glCompileShader(fs);

    glAttachShader(shader_id, vs);
    glAttachShader(shader_id, fs);
    glLinkProgram(shader_id);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(shader_id);
    glUniform1i(glGetUniformLocation(shader_id, "skybox"), 0);

    // -------- VAO/VBO --------
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
void Skybox::render(const Camera& camera)
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader_id);

    // ===== VIEW (SIN TRASLACIÓN) =====
    float dx, dy, dz;
    camera.getDirection(dx, dy, dz);

    glm::mat4 view = glm::mat4(glm::mat3(
        glm::lookAt(glm::vec3(0.0f),
            glm::normalize(glm::vec3(dx, dy, dz)),
            glm::vec3(0, 1, 0))
    ));

    glm::mat4 projection = glm::perspective(
        glm::radians(45.f),
        1024.f / 576.f,
        0.1f,
        5000.f
    );

    // ===== UNIFORMS (OPTIMIZADO: sin llamadas repetidas) =====
    static GLint viewLoc = glGetUniformLocation(shader_id, "view");
    static GLint projLoc = glGetUniformLocation(shader_id, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // ===== DRAW =====
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
    glDeleteProgram(shader_id);
}