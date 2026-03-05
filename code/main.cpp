#include "Scene.hpp"
#include <Window.hpp>
#include <SDL3/SDL.h>  // SDL3 correcta


using udit::Scene;
using udit::Window;

int main(int, char* [])
{
    constexpr unsigned viewport_width = 1024;
    constexpr unsigned viewport_height = 576;

    Window window("OpenGL example", viewport_width, viewport_height, { 3, 3 });
    Scene scene(viewport_width, viewport_height);

    bool exit_app = false;
    int mouse_x = 0;
    int mouse_y = 0;
    bool button_down = false;

    // Bucle principal
    while (!exit_app)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT && !button_down)
                {
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                    scene.on_click(static_cast<float>(mouse_x), static_cast<float>(mouse_y), button_down = true);
                }
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                if (event.button.button == SDL_BUTTON_LEFT && button_down)
                {
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                    scene.on_click(static_cast<float>(mouse_x), static_cast<float>(mouse_y), button_down = false);
                }
                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                scene.on_drag(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
                break;
            }

            case SDL_EVENT_QUIT:
            {
                exit_app = true;
                break;
            }
            }
        }

        // Actualizar la escena
        scene.update();

        // Renderizar la escena
        scene.render();

        // Intercambiar buffers
        window.swap_buffers();
    }

    SDL_Quit();
    return 0;
}