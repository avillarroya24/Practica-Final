#include "Skybox.hpp"
#include <cassert>
#include <iostream>
#include <stb_image.h>

namespace udit {

    using namespace glm;
    using namespace std;

    const GLfloat Skybox::coordinates[] = {
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

    const std::string Skybox::vertex_shader_code = R"(
#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
out vec3 TexCoords;
void main() {
    TexCoords = position;
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
}
)";

    const std::string Skybox::fragment_shader_code = R"(
#version 330 core
in vec3 TexCoords;
out vec4 FragColor;
uniform samplerCube sampler;
void main() {
    FragColor = texture(sampler, TexCoords);
}
)";

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

    Skybox::Skybox()
    {
        std::vector<std::string> faces_paths = {
            "sky-cube-map-6_right.png",   // +X
            "sky-cube-map-6_left.png",    // -X
            "sky-cube-map-6_top.png",     // +Y
            "sky-cube-map-6_bottom.png",  // -Y
            "sky-cube-map-6_front.png",   // +Z
            "sky-cube-map-6_back.png"     // -Z
        };
        *this = Skybox(faces_paths); // reutiliza el constructor principal
    }

    Skybox::~Skybox()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(1, &vbo_id);
        glDeleteTextures(1, &cubemap_texture_id);
    }

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

    GLuint Skybox::loadCubemap(const std::vector<std::string>& faces_paths)
    {
        assert(faces_paths.size() == 6);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (GLuint i = 0; i < faces_paths.size(); i++)
        {
            unsigned char* data = stbi_load(faces_paths[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

    GLuint Skybox::compile_shaders()
    {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vsrc = vertex_shader_code.c_str();
        const char* fsrc = fragment_shader_code.c_str();

        glShaderSource(vs, 1, &vsrc, nullptr);
        glShaderSource(fs, 1, &fsrc, nullptr);

        glCompileShader(vs);
        glCompileShader(fs);

        GLint success;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) show_compilation_error(vs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success) show_compilation_error(fs);

        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) show_linkage_error(program);

        return program;
    }

    void Skybox::show_compilation_error(GLuint shader_id)
    {
        GLint length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> info(length);
        glGetShaderInfoLog(shader_id, length, nullptr, info.data());
        std::cerr << info.data() << std::endl;
        assert(false);
    }

    void Skybox::show_linkage_error(GLuint program_id)
    {
        GLint length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> info(length);
        glGetProgramInfoLog(program_id, length, nullptr, info.data());
        std::cerr << info.data() << std::endl;
        assert(false);
    }

} // namespace udit