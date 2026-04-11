#include "Texture_Cube.hpp"

#include <SOIL2.h>
#include <iostream>
#include <vector>

namespace udit
{

    Texture_Cube::Texture_Cube(const std::string& texture_base_path)
        : texture_id(0), texture_is_loaded(false)
    {
        // =========================
        // CARGA DE LAS 6 CARAS
        // =========================
        std::vector<unsigned char*> sides(6);

        for (size_t i = 0; i < 6; i++)
        {
            std::string path = texture_base_path + std::to_string(i) + ".png";

            int width, height, channels;

            sides[i] = SOIL_load_image(
                path.c_str(),
                &width,
                &height,
                &channels,
                SOIL_LOAD_RGBA
            );

            if (!sides[i])
            {
                std::cerr << "Error cargando cubemap: " << path << std::endl;

                for (size_t j = 0; j < i; j++)
                {
                    SOIL_free_image_data(sides[j]);
                }

                return;
            }

            // Guardamos tamaño solo una vez (opcional si quieres extenderlo luego)
            if (i == 0)
            {
                tex_width = width;
                tex_height = height;
            }
        }

        // =========================
        // CREAR TEXTURA CUBEMAP
        // =========================
        glEnable(GL_TEXTURE_CUBE_MAP);

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // =========================
        // ORDEN DE CARAS (IMPORTANTE)
        // =========================
        static const GLenum targets[6] =
        {
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        };

        // =========================
        // SUBIR A GPU
        // =========================
        for (size_t i = 0; i < 6; i++)
        {
            glTexImage2D(
                targets[i],
                0,
                GL_RGBA,
                tex_width,
                tex_height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                sides[i]
            );

            SOIL_free_image_data(sides[i]);
        }

        texture_is_loaded = true;
    }

    // =========================
    // DESTRUCTOR
    // =========================
    Texture_Cube::~Texture_Cube()
    {
        if (texture_is_loaded)
        {
            glDeleteTextures(1, &texture_id);
        }
    }

}