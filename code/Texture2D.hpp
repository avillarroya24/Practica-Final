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
        bool loaded = false;

        int width = 0;
        int height = 0;

    public:
        // ============================
        // CONSTRUCTOR / DESTRUCTOR
        // ============================
        Texture2D(const std::string& path);
        ~Texture2D();

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        // ============================
        // INFO
        // ============================
        bool is_loaded() const { return loaded; }

        GLuint get_id() const { return texture_id; }

        int get_width() const { return width; }
        int get_height() const { return height; }

        // ============================
        // BIND CORRECTO (CLAVE)
        // ============================
        void bind(unsigned int slot = 0) const;

        // ============================
        // GLOBAL SYSTEM
        // ============================
        static Texture2D* load(const std::string& name, const std::string& path);
        static Texture2D* get(const std::string& name);
        static void load_default_textures();
    };
}

#endif