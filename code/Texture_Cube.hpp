#ifndef TEXTURE_CUBE_HEADER
#define TEXTURE_CUBE_HEADER

#include <string>
#include <glad/gl.h>
#include <GL/gl.h>

namespace udit
{

    class Texture_Cube
    {
    private:
        GLuint texture_id = 0;
        bool texture_is_loaded = false;

    public:
        Texture_Cube(const std::string& texture_base_path);
        ~Texture_Cube();

        Texture_Cube(const Texture_Cube&) = delete;
        Texture_Cube& operator = (const Texture_Cube&) = delete;

    public:
        bool is_ok() const
        {
            return texture_is_loaded;
        }

        bool bind() const
        {
            return texture_is_loaded ? (glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id), true) : false;
        }
    };

}

#endif