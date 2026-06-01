#include "Cube.hpp"
#include <glm.hpp>

/*

    Implementación de la clase Cube.

 * Este archivo define la geometría de un cubo y su comportamiento
 * en renderizado usando OpenGL. Incluye la gestión de buffers,
 * colores dinámicos y texturas.

*/

namespace udit
{
    // ================= VÉRTICES =================

    /*
        Coordenadas de los vértices del cubo.

     * Define las posiciones en 3D de los 8 vértices del cubo.
    */
    const GLfloat Cube::coordinates[] =
    {
        -1,-1, 1,   1,-1, 1,   1, 1, 1,  -1, 1, 1,
        -1,-1,-1,   1,-1,-1,   1, 1,-1,  -1, 1,-1
    };

    // ================= ÍNDICES =================

    /*
        
        Índices de los triángulos del cubo.

     * Define cómo se conectan los vértices para formar las caras
       del cubo mediante triángulos.
    */
    const GLubyte Cube::indices[] =
    {
        0,1,2, 0,2,3,
        1,5,6, 1,6,2,
        5,4,7, 5,7,6,
        4,0,3, 4,3,7,
        3,2,6, 3,6,7,
        4,5,1, 4,1,0
    };

    // ================= UVs =================

    /*
    
        Coordenadas de textura (UV) del cubo.

     * Define cómo se mapea una textura sobre las caras del cubo.
    */
    const GLfloat Cube::texcoords[] =
    {
        0,0, 1,0, 1,1, 0,1,
        0,0, 1,0, 1,1, 0,1
    };

    // ================= CONSTRUCTOR =================

    /*
    
        Constructor del cubo.

     * Inicializa los buffers de OpenGL (VAO, VBO, EBO) y configura
     * los atributos de vértices: posición, color y coordenadas UV.
    */

    Cube::Cube()
    {
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(VBO_COUNT, vbo_ids);

        glBindVertexArray(vao_id);

        // POSICIONES
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // COLORES
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        // UVs
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORDS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        // ÍNDICES
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        use_texture = false;
        texture_id = 0;
    }

    // ================= DESTRUCTOR =================

    //Libera los recursos de OpenGL asociados al cubo
    Cube::~Cube()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }

    // ================= COLOR DINÁMICO =================

    /*
    
        Establece el color del cubo.
     
     * Actualiza dinámicamente el color de todos los vértices.
    
     * @param color Vector RGB con el color deseado.
    
    */

    void Cube::set_color(const glm::vec3& color)
    {
        GLfloat data[24];

        for (int i = 0; i < 8; i++)
        {
            data[i * 3 + 0] = color.r;
            data[i * 3 + 1] = color.g;
            data[i * 3 + 2] = color.b;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COLORS_VBO]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
    }

    // ================= TEXTURA =================

    /*
    
         Asigna una textura al cubo.
 
     * @param texture ID de la textura en OpenGL.
    */
    void Cube::set_texture(GLuint texture)
    {
        texture_id = texture;
    }

    /*
    
        Activa o desactiva el uso de textura.
    
     * @param enable true para usar textura, false para desactivarla.
    */

    void Cube::enable_texture(bool enable)
    {
        use_texture = enable;
    }

    // ================= RENDER =================

    /*
    
         Renderiza el cubo.

     * Aplica la transformación del modelo, enlaza la textura si está activa
       y dibuja el cubo utilizando los índices definidos. También renderiza
       recursivamente los objetos hijos.
    
    */
    void Cube::render()
    {
        glm::mat4 model = transform.get_transform_matrix();

        glBindVertexArray(vao_id);

        if (use_texture && texture_id != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);

        glBindVertexArray(0);

        for (auto* child : children)
            if (child)
                child->render();
    }
}