#pragma once
#include <glad/gl.h>
#include <string>

class Shader
{
public:
    GLuint id;

    Shader(const char* vs, const char* fs);
    void use() const;
    void setMat4(const char* name, const float* value) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setInt(const char* name, int v) const;
    void setVec4(const char* name, float x, float y, float z, float w) const;
};
