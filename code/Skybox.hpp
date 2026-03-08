#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <string>

namespace udit {

    class Skybox {
    private:
        // 1) Declaración correcta del array estático
        static const GLfloat coordinates[];

        // 2) Declaración correcta de los shaders
        static const std::string vertex_shader_code;
        static const std::string fragment_shader_code;

        // Buffers
        GLuint vbo_id = 0;
        GLuint vao_id = 0;

        // Shader y textura
        GLuint shader_program_id = 0;
        GLuint cubemap_texture_id = 0;

        // Uniforms
        GLint model_view_matrix_id = -1;
        GLint projection_matrix_id = -1;

    public:
        // Constructor recibe las 6 rutas de las texturas del cubemap
        Skybox(const std::vector<std::string>& faces_paths);
        ~Skybox();

        // Renderiza el skybox, recibiendo la matriz view y projection actuales
        void render(const glm::mat4& view, const glm::mat4& projection);

    private:
        GLuint compile_shaders();
        void show_compilation_error(GLuint shader_id);
        void show_linkage_error(GLuint program_id);

        // 3) Carga del cubemap
        GLuint loadCubemap(const std::vector<std::string>& faces_paths);
    };

}  // namespace udit

#endif
