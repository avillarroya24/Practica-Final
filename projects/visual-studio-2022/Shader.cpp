#include "Shader.hpp"
#include <iostream>

static GLuint compile(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        GLint len;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, ' ');
        glGetShaderInfoLog(s, len, nullptr, log.data());
        std::cerr << "Shader error: " << log << std::endl;
        exit(1);
    }
    return s;
}

Shader::Shader(const char* vs, const char* fs)
{
    GLuint v = compile(GL_VERTEX_SHADER, vs);
    GLuint f = compile(GL_FRAGMENT_SHADER, fs);

    id = glCreateProgram();
    glAttachShader(id, v);
    glAttachShader(id, f);
    glLinkProgram(id);

    GLint ok;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        GLint len;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, ' ');
        glGetProgramInfoLog(id, len, nullptr, log.data());
        std::cerr << "Program error: " << log << std::endl;
        exit(1);
    }

    glDeleteShader(v);
    glDeleteShader(f);
}

void Shader::use() const { glUseProgram(id); }

void Shader::setMat4(const char* name, const float* v) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, v);
}

void Shader::setVec3(const char* name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void Shader::setInt(const char* name, int v) const
{
    glUniform1i(glGetUniformLocation(id, name), v);
}
