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
const char* Skybox::vertex_shader_code =
"#version 330 core\n"
"layout(location = 0) in vec3 vertex_coordinates;\n"
"out vec3 tex_coords;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main() {\n"
"    tex_coords = vertex_coordinates;\n"
"    vec4 pos = projection * view * vec4(vertex_coordinates, 1.0);\n"
"    gl_Position = pos.xyww;\n"
"}";

const char* Skybox::fragment_shader_code =
"#version 330 core\n"
"in vec3 tex_coords;\n"
"out vec4 FragColor;\n"
"uniform samplerCube skybox;\n"
"void main() {\n"
"    FragColor = texture(skybox, tex_coords);\n"
"}";

// ================= CUBEMAP =================
GLuint Skybox::loadCubemap(const std::string& texture_path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

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

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGBA, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, data);

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
    // 1. Cargar cubemap
    cubemap_texture = loadCubemap(texture_path);

    // 2. Compilar shader del skybox
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

    // 3. Asignar samplerCube a la unidad 0
    glUseProgram(shader_id);
    glUniform1i(glGetUniformLocation(shader_id, "skybox"), 0);

    // 4. Crear VAO/VBO
    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_id);

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
}

void Skybox::render(const Camera& camera)
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader_id);

    // ===== OBTENER DATOS DE TU CÁMARA =====
    glm::vec3 camPos(camera.getX(), camera.getY(), camera.getZ());

    float dx, dy, dz;
    camera.getDirection(dx, dy, dz);

    glm::vec3 front = glm::normalize(glm::vec3(dx, dy, dz));
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    // IMPORTANTE: SIN traslación
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f), front, up);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.f),
        1024.0f / 576.0f,
        0.1f,
        5000.0f
    );

    // ===== PASAR MATRICES =====
    glUniformMatrix4fv(glGetUniformLocation(shader_id, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader_id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // ===== DIBUJO =====
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
