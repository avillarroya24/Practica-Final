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
        static const GLfloat coordinates[];
        static const std::string vertex_shader_code;
        static const std::string fragment_shader_code;

        GLuint vbo_id = 0;
        GLuint vao_id = 0;

        GLuint shader_program_id = 0;
        GLuint cubemap_texture_id = 0;

        GLint model_view_matrix_id = -1;
        GLint projection_matrix_id = -1;

    public:
        Skybox(const std::vector<std::string>& faces_paths); // Constructor con rutas personalizadas
        Skybox(); // Constructor por defecto con Sky Cube Map 6
        ~Skybox();

        void render(const glm::mat4& view, const glm::mat4& projection);

    private:
        GLuint compile_shaders();
        void show_compilation_error(GLuint shader_id);
        void show_linkage_error(GLuint program_id);

        GLuint loadCubemap(const std::vector<std::string>& faces_paths);
    };

} // namespace udit

#endif