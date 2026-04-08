#include "Texture_Cube.hpp"
#include <SOIL2.h>
#include <iostream>

namespace udit
{

    Texture_Cube::Texture_Cube(const std::string& texture_path)
        : texture_id(0), texture_is_loaded(false)
    {
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        // Cargar la misma textura en las 6 caras
        for (unsigned i = 0; i < 6; ++i)
        {
            int width, height, channels;
            unsigned char* data = SOIL_load_image(texture_path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

            if (!data)
            {
                std::cerr << "Error cargando textura: " << texture_path << std::endl;
                texture_is_loaded = false;
                glDeleteTextures(1, &texture_id);
                return;
            }

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGBA,
                width,
                height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );

            SOIL_free_image_data(data);
        }

        // Parámetros del cubemap
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        texture_is_loaded = true;
    }

    Texture_Cube::~Texture_Cube()
    {
        if (texture_id != 0)
        {
            glDeleteTextures(1, &texture_id);
        }
    }

}