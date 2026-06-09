#include "Shader_Program.hpp"
#include <iostream>

/*

   Implementación de la clase Shader_Program.
 
 * Este archivo gestiona la creación, compilación y uso de programas de shaders
   en OpenGL. Un programa de shaders está formado por un vertex shader y un
   fragment shader que se compilan y enlazan.
    

*/


// ================= CONSTRUCTOR =================

/*

    Constructor del programa de shaders.
 
 * Recibe el código fuente del vertex shader y fragment shader,
   y los compila automáticamente.
 
 * @param vertex_shader_code Código GLSL del vertex shader.
 * @param fragment_shader_code Código GLSL del fragment shader.

*/

Shader_Program::Shader_Program(
    const std::string& vertex_shader_code,
    const std::string& fragment_shader_code
)
    : vertex_code(vertex_shader_code),
    fragment_code(fragment_shader_code)
{
    compile_shaders();
}

// ================= VALIDACIÓN =================

/*

   Comprueba si el shader se compiló correctamente.

 * true si el programa es válido, false en caso contrario.

*/

bool Shader_Program::good() const
{
    return shader_program_id != -1;
}

// ================= USO =================

/*
   Activa el programa de shaders.
 
 * Hace que OpenGL utilice este shader para el renderizado.
*/

void Shader_Program::use()
{
    if (good())
        glUseProgram(shader_program_id);
}

// ================= UNIFORMS =================

/*

    Obtiene la localización de una variable uniform.
 
 * name Nombre del uniform en el shader.
 * Identificador de la ubicación del uniform.

*/

GLint Shader_Program::get_uniform_location(const char* name)
{
    return glGetUniformLocation(shader_program_id, name);
}

// ================= COMPILACIÓN =================

/*

   Compila y enlaza los shaders.
 
 * Este método:
 * - Compila el vertex shader
 * - Compila el fragment shader
 * - Enlaza ambos en un programa OpenGL
 * - Comprueba errores en cada etapa
 
*/

void Shader_Program::compile_shaders()
{
    GLint success;
    char infoLog[512];

    // ==========================
    // ===== VERTEX SHADER ======
    // ==========================

    //Se crea y compila el vertex shader

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char* v_code = vertex_code.c_str();

    glShaderSource(vs, 1, &v_code, nullptr);
    glCompileShader(vs);

    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vs, 512, nullptr, infoLog);
        std::cout << "Vertex shader error:\n" << infoLog << std::endl;
        shader_program_id = -1;
        return;
    }

    // ==========================
    // ===== FRAGMENT SHADER ====
    // ==========================

    //Se crea y compila el fragment shader

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* f_code = fragment_code.c_str();

    glShaderSource(fs, 1, &f_code, nullptr);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fs, 512, nullptr, infoLog);
        std::cout << "Fragment shader error:\n" << infoLog << std::endl;
        shader_program_id = -1;
        return;
    }

    // ==========================
    // ===== PROGRAM ============
    // ==========================

    //Se enlazan ambos shaders en un programa ejecutable

    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vs);
    glAttachShader(shader_program_id, fs);
    glLinkProgram(shader_program_id);

    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program_id, 512, nullptr, infoLog);
        std::cout << "Shader link error:\n" << infoLog << std::endl;
        shader_program_id = -1;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}