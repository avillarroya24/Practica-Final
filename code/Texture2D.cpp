#include "Texture2D.hpp"

#include <SOIL2.h>
#include <iostream>
#include <unordered_map>

namespace udit
{
    // ============================
    // ALMACÉN GLOBAL DE TEXTURAS
    // ============================
    static std::unordered_map<std::string, Texture2D*> texture_bank;

    // ============================
    // CARGA INDIVIDUAL
    // ============================
    Texture2D::Texture2D(const std::string& path)
    {
        int channels;

        unsigned char* data = SOIL_load_image(
            path.c_str(),
            &tex_width,
            &tex_height,
            &channels,
            SOIL_LOAD_RGBA
        );

        if (!data)
        {
            std::cerr << "Error cargando textura: " << path << std::endl;
            texture_is_loaded = false;
            return;
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            tex_width,
            tex_height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        SOIL_free_image_data(data);

        texture_is_loaded = true;
    }

    // ============================
    // DESTRUCTOR
    // ============================
    Texture2D::~Texture2D()
    {
        if (texture_is_loaded)
        {
            glDeleteTextures(1, &texture_id);
        }
    }

    // ============================
    // CARGA CENTRALIZADA
    // ============================
    Texture2D* Texture2D::load(const std::string& name, const std::string& path)
    {
        Texture2D* tex = new Texture2D(path);

        if (tex->is_loaded())
        {
            texture_bank[name] = tex;
            return tex;
        }

        delete tex;
        return nullptr;
    }

    // ============================
    // ACCESO A TEXTURAS
    // ============================
    Texture2D* Texture2D::get(const std::string& name)
    {
        if (texture_bank.count(name))
            return texture_bank[name];

        return nullptr;
    }

    // ============================
    // CARGA DE TUS TEXTURAS AQUÍ
    // ============================
    void Texture2D::load_default_textures()
    {
        load("circulo", "../../shared/textures/circulo-oscuro.png");
        load("earth", "../../shared/textures/earth_daymap.png");
        load("luna", "../../shared/textures/luna.png");
        load("sky6", "../../shared/textures/sky-cube-map-6.png");
        load("terrain", "../../shared/textures/terrain.png");

        std::cout << "Texturas 2D cargadas correctamente." << std::endl;
    }

}
