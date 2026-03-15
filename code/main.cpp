#include "Scene.hpp"
#include "Window.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

using udit::Scene;
using udit::Window;

int main(int argc, char* argv[])
{
    constexpr unsigned viewport_width = 1024;
    constexpr unsigned viewport_height = 576;

    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Error al inicializar SDL: %s", SDL_GetError());
        return -1;
    }

    Window window("OpenGL example", viewport_width, viewport_height, { 3, 3 });
    Scene  scene(viewport_width, viewport_height);

    bool exit = false;

    // Game loop
    while (!exit)
    {
        SDL_Event event;

        // Procesar eventos
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                exit = true;
        }

        // Actualizar la escena
        scene.update();

        // Renderizar la escena
        scene.render();

        // Mostrar en pantalla
        window.swap_buffers();
    }

    SDL_Quit();

    return 0;
}