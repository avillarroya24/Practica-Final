#include <cassert>
#include <iostream>
#include <glad/gl.h>
#include "Skybox.hpp"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <stb_image.h>

namespace udit {

    using namespace std;
    using namespace glm;

    // ------------------------------
    //  DEFINICIÓN DE COORDENADAS
    // ------------------------------
    const GLfloat Skybox::coordinates[] =
    {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // ------------------------------
    //  SHADERS DEL SKYBOX
    // ------------------------------
    const std::string Skybox::vertex_shader_code = R"(

#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

out vec3 TexCoords;

void main()
{
    TexCoords = position;
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
}

)";

    const std::string Skybox::fragment_shader_code = R"(

#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube sampler;

void main()
{
    FragColor = texture(sampler, TexCoords);
}

)";

    // ------------------------------
    //  CONSTRUCTOR
    // ------------------------------
    Skybox::Skybox(const std::vector<std::string>& faces_paths)
    {
        shader_program_id = compile_shaders();
        cubemap_texture_id = loadCubemap(faces_paths);

        model_view_matrix_id = glGetUniformLocation(shader_program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(shader_program_id, "projection_matrix");

        glGenBuffers(1, &vbo_id);
        glGenVertexArrays(1, &vao_id);

        glBindVertexArray(vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
    }

    // ------------------------------
    //  DESTRUCTOR
    // ------------------------------
    Skybox::~Skybox()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(1, &vbo_id);
        glDeleteTextures(1, &cubemap_texture_id);
    }

    // ------------------------------
    //  RENDER
    // ------------------------------
    void Skybox::render(const mat4& view, const mat4& projection)
    {
        glUseProgram(shader_program_id);

        mat4 view_no_translation = mat4(mat3(view));
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, value_ptr(view_no_translation));
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_id);
        glUniform1i(glGetUniformLocation(shader_program_id, "sampler"), 0);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glBindVertexArray(vao_id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    // ------------------------------
    //  CARGA DE CUBEMAP
    // ------------------------------
    GLuint Skybox::loadCubemap(const std::vector<std::string>& faces_paths)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;

        for (GLuint i = 0; i < faces_paths.size(); i++)
        {
            unsigned char* data = stbi_load(faces_paths[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cerr << "Failed to load cubemap face: " << faces_paths[i] << std::endl;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

} // namespace udit
