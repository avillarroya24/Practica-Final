#ifndef TEXTURE_CUBE_HEADER
#define TEXTURE_CUBE_HEADER

#include <string>
#include <glad/gl.h>

namespace udit
{

    class Texture_Cube
    {
    private:
        GLuint texture_id = 0;
        bool texture_is_loaded = false;

        // (Opcional pero útil si quieres extender el sistema)
        int tex_width = 0;
        int tex_height = 0;

    public:
        // Constructor: carga cubemap desde base path (0.png ... 5.png)
        Texture_Cube(const std::string& texture_base_path);

        // Destructor
        ~Texture_Cube();

        // No copia
        Texture_Cube(const Texture_Cube&) = delete;
        Texture_Cube& operator=(const Texture_Cube&) = delete;

    public:
        // Estado de carga
        bool is_ok() const
        {
            return texture_is_loaded;
        }

        // Bind seguro
        bool bind() const
        {
            if (!texture_is_loaded)
                return false;

            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
            return true;
        }
    };

}

#endif