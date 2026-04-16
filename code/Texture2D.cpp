#include "Texture2D.hpp"

#include <SOIL2.h>
#include <iostream>

namespace udit
{

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
            return;
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

    Texture2D::~Texture2D()
    {
        if (texture_is_loaded)
        {
            glDeleteTextures(1, &texture_id);
        }
    }

}