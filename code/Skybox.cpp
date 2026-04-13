#include "Skybox.hpp"
#include <iostream>
#include <SOIL2.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// ================= CUBO (36 vértices) =================
const GLfloat Skybox::coordinates[] =
{
    -1.0f, +1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,
    +1.0f, +1.0f, -1.0f,
    -1.0f, +1.0f, -1.0f,

    -1.0f, -1.0f, +1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, +1.0f, -1.0f,
    -1.0f, +1.0f, -1.0f,
    -1.0f, +1.0f, +1.0f,
    -1.0f, -1.0f, +1.0f,

    +1.0f, -1.0f, -1.0f,
    +1.0f, -1.0f, +1.0f,
    +1.0f, +1.0f, +1.0f,
    +1.0f, +1.0f, +1.0f,
    +1.0f, +1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, +1.0f,
    -1.0f, +1.0f, +1.0f,
    +1.0f, +1.0f, +1.0f,
    +1.0f, +1.0f, +1.0f,
    +1.0f, -1.0f, +1.0f,
    -1.0f, -1.0f, +1.0f,

    -1.0f, +1.0f, -1.0f,
    +1.0f, +1.0f, -1.0f,
    +1.0f, +1.0f, +1.0f,
    +1.0f, +1.0f, +1.0f,
    -1.0f, +1.0f, +1.0f,
    -1.0f, +1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, +1.0f,
    +1.0f, -1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, +1.0f,
    +1.0f, -1.0f, +1.0f,
};

// ================= SHADERS =================
const char* Skybox::vertex_shader_code =
"#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"out vec3 TexCoords;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main() {\n"
"    TexCoords = vec3(aPos.x, -aPos.y, aPos.z);\n"
"    gl_Position = projection * view * vec4(aPos, 1.0);\n"
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
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    std::vector<std::string> faces =
    {
        base_path + "/right.jpg",
        base_path + "/left.jpg",
        base_path + "/top.jpg",
        base_path + "/bottom.jpg",
        base_path + "/front.jpg",
        base_path + "/back.jpg"
    };

    int width, height, channels;

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = SOIL_load_image(
            faces[i].c_str(),
            &width, &height, &channels,
            SOIL_LOAD_RGBA
        );

        if (!data)
        {
            std::cerr << "Error cargando cara del cubemap: " << faces[i] << std::endl;
            glDeleteTextures(1, &texID);
            return 0;
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGBA,
            width, height,
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

    return texID;
}

// ================= CONSTRUCTOR =================
Skybox::Skybox(const std::string& path)
{
    cubemap_texture = loadCubemap(path);

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
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(shader_id);



    // ===== PROYECCIÓN REAL DE LA CÁMARA =====
    glm::mat4 projection = camera.get_projection_matrix();

    static GLint viewLoc = glGetUniformLocation(shader_id, "view");
    static GLint projLoc = glGetUniformLocation(shader_id, "projection");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
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
