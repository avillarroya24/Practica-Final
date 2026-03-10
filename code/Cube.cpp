#include "Cube.hpp"

const GLfloat Cube::coordinates[] =
{
-0.5,-0.5,0.5,
0.5,-0.5,0.5,
0.5,0.5,0.5,
-0.5,0.5,0.5,

-0.5,-0.5,-0.5,
0.5,-0.5,-0.5,
0.5,0.5,-0.5,
-0.5,0.5,-0.5
};

const GLfloat Cube::colors[] =
{
1,0,0,
0,1,0,
0,0,1,
1,1,0,

1,0,1,
0,1,1,
1,1,1,
0,0,0
};

const GLubyte Cube::indices[] =
{
0,1,2, 2,3,0,
1,5,6, 6,2,1,
5,4,7, 7,6,5,
4,0,3, 3,7,4,
3,2,6, 6,7,3,
4,5,1, 1,0,4
};

Cube::Cube()
{
    create_buffers(
        coordinates, sizeof(coordinates),
        colors, sizeof(colors),
        indices, sizeof(indices)
    );
}