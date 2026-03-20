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
"void main()\n"
"{\n"
"    tex_coords = vertex_coordinates;\n"
"    vec4 pos = projection * view * vec4(vertex_coordinates, 1.0);\n"
"    gl_Position = pos.xyww;\n"
"}";

const char* Skybox::fragment_shader_code =
"#version 330 core\n"
"in vec3 tex_coords;\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"    // Dirección normalizada del cubo\n"
"    vec3 dir = normalize(tex_coords);\n"
"\n"
"    // Altura del cielo (0 = horizonte, 1 = zenit)\n"
"    float h = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);\n"
"\n"
"    // Azul oscuro del horizonte\n"
"    vec3 horizon = vec3(0.02, 0.05, 0.10);\n"
"\n"
"    // Azul profundo del cielo superior\n"
"    vec3 zenith = vec3(0.00, 0.02, 0.06);\n"
"\n"
"    // Mezcla suave según la altura\n"
"    vec3 color = mix(horizon, zenith, h);\n"
"\n"
"    FragColor = vec4(color, 1.0);\n"
"}";



// ================= CUBEMAP =================
GLuint Skybox::loadCubemap(const std::string& base_path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    std::string faces[6] = {
        base_path + "0.png",
        base_path + "1.png",
        base_path + "2.png",
        base_path + "3.png",
        base_path + "4.png",
        base_path + "5.png"
    };

    int width, height, channels;

    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = SOIL_load_image(
            faces[i].c_str(),
            &width,
            &height,
            &channels,
            SOIL_LOAD_AUTO
        );

        if (data)
        {
            GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                format,
                width,
                height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data
            );

            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "❌ Error cargando: " << faces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// ================= CONSTRUCTOR =================
Skybox::Skybox(const std::string& texture_base_path)
{
    cubemap_texture = loadCubemap(texture_base_path);

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

    glUniformMatrix4fv(glGetUniformLocation(shader, "view"),
        1, GL_FALSE, glm::value_ptr(view_no_translation));

    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"),
        1, GL_FALSE, glm::value_ptr(projection));


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
