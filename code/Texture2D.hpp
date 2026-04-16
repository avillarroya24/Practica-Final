#ifndef TEXTURE2D_HEADER
#define TEXTURE2D_HEADER

#include <string>
#include <glad/gl.h>

namespace udit
{

    class Texture2D
    {
    private:
        GLuint texture_id = 0;
        bool texture_is_loaded = false;

        int tex_width = 0;
        int tex_height = 0;

    public:
        Texture2D(const std::string& path);
        ~Texture2D();

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        bool is_ok() const
        {
            return texture_is_loaded;
        }

        bool bind(unsigned int unit) const
        {
            if (!texture_is_loaded)
                return false;

            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            return true;
        }
    };

}

#endif