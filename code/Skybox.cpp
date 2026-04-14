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
"    TexCoords = aPos;\n"
"    vec4 pos = projection * vec4((view * vec4(aPos, 0.0)).xyz, 1.0);\n"
"    gl_Position = pos;\n"
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

    // base_path viene como "../../../shared/assets/sky-cube-map-"
    // así que los ficheros son:
    // sky-cube-map-0.jpg ... sky-cube-map-5.jpg
    std::vector<std::string> faces =
    {
        base_path + "0.jpg", // +X
        base_path + "1.jpg", // -X
        base_path + "2.jpg", // +Y
        base_path + "3.jpg", // -Y
        base_path + "4.jpg", // +Z
        base_path + "5.jpg"  // -Z
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

    // Desactivar culling para ver el cubo desde dentro
    GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
    if (cullEnabled) glDisable(GL_CULL_FACE);

    glUseProgram(shader_id);

    glm::mat4 projection = camera.get_projection_matrix();
    glm::mat4 view = glm::mat4(glm::mat3(camera.get_view_matrix()));

    static GLint viewLoc = glGetUniformLocation(shader_id, "view");
    static GLint projLoc = glGetUniformLocation(shader_id, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    // Restaurar estado
    if (cullEnabled) glEnable(GL_CULL_FACE);

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
