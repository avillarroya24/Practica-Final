#ifndef SCENE_HEADER
#define SCENE_HEADER

#include "Camera.hpp"
#include "Skybox.hpp"

/*
    Aqui se añaden las variables de todo tipo int, float y tambien se añaden la camara y el skybox
*/

namespace udit
{

    class Scene
    {
    private:

        Camera camera;
        Skybox skybox;

        int    width;
        int    height;

        float  angle_around_x;
        float  angle_around_y;
        float  angle_delta_x;
        float  angle_delta_y;

        bool   pointer_pressed;
        float  last_pointer_x;
        float  last_pointer_y;

        // Movimiento libre
        bool move_forward;
        bool move_backward;
        bool move_left;
        bool move_right;
        float camera_speed;

    public:

        Scene(int width, int height);

        void update();
        void render();

        /*
            Aqui se hace lo del raton para poder dar en la escene y directamente poder girar la camara
        */

    public:

        void resize(int width, int height);
        void on_drag(float pointer_x, float pointer_y);
        void on_click(float pointer_x, float pointer_y, bool down);

    };

}

#endif
