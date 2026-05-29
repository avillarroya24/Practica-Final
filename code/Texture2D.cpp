#include "Texture2D.hpp"

#include <SOIL2.h>
#include <iostream>
#include <unordered_map>

namespace udit
{
    static std::unordered_map<std::string, Texture2D*> texture_bank;

    Texture2D::Texture2D(const std::string& path)
    {
        loaded = false;

        int channels = 0;

        unsigned char* data = SOIL_load_image(
            path.c_str(),
            &width,
            &height,
            &channels,
            SOIL_LOAD_AUTO   // 🔥 IMPORTANTE: respeta formato real
        );

        if (!data)
        {
            std::cerr << "[ERROR] No se pudo cargar textura: " << path << std::endl;

            unsigned char white[4] = { 255, 255, 255, 255 };
            width = height = 1;

            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, white);

            glBindTexture(GL_TEXTURE_2D, 0);

            loaded = true;
            return;
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        // 🔥 WRAP seguro
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // 🔥 FILTROS correctos
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 🔥 FIX mipmaps en drivers modernos
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        SOIL_free_image_data(data);

        loaded = true;

        std::cout << "[OK] Textura cargada: " << path
            << " (" << width << "x" << height << ", "
            << channels << " canales)\n";
    }

    Texture2D::~Texture2D()
    {
        if (texture_id != 0)
            glDeleteTextures(1, &texture_id);
    }

    void Texture2D::bind(unsigned int slot) const
    {
        if (!loaded || texture_id == 0) return;

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

    Texture2D* Texture2D::load(const std::string& name, const std::string& path)
    {
        if (texture_bank.count(name))
            return texture_bank[name];

        Texture2D* tex = new Texture2D(path);

        if (tex->is_loaded())
        {
            texture_bank[name] = tex;
            return tex;
        }

        delete tex;
        return nullptr;
    }

    Texture2D* Texture2D::get(const std::string& name)
    {
        auto it = texture_bank.find(name);

        if (it != texture_bank.end())
            return it->second;

        std::cerr << "[WARN] Textura no encontrada: " << name << std::endl;
        return nullptr;
    }

    void Texture2D::load_default_textures()
    {
        std::cout << "Cargando texturas...\n";

        load("circulo", "textures/circulo-oscuro.png");
        load("earth", "textures/earth_daymap.png");
        load("luna", "textures/luna.png");
        load("terrain", "textures/terrain.png");

        std::cout << "Texturas cargadas.\n";
    }
}