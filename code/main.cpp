#include "Scene.hpp"
#include "Window.hpp"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

using udit::Scene;
using udit::Window;

int main(int, char* [])
{
    constexpr unsigned WIDTH = 1024;
    constexpr unsigned HEIGHT = 576;

    Window window("OpenGL example", WIDTH, HEIGHT, { 3, 3 });
    Scene scene(WIDTH, HEIGHT);

    bool running = true;
    bool first_mouse = true;

    float mouse_x = 0.0f, mouse_y = 0.0f;
    float last_mouse_x = WIDTH * 0.5f;
    float last_mouse_y = HEIGHT * 0.5f;

    while (running)
    {
        SDL_Event event;

        float mouse_dx = 0.0f;
        float mouse_dy = 0.0f;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_MOUSE_MOTION:
            {
                SDL_GetMouseState(&mouse_x, &mouse_y);

                if (first_mouse)
                {
                    last_mouse_x = mouse_x;
                    last_mouse_y = mouse_y;
                    first_mouse = false;
                }

                mouse_dx += (mouse_x - last_mouse_x);
                mouse_dy += (mouse_y - last_mouse_y);

                last_mouse_x = mouse_x;
                last_mouse_y = mouse_y;
                break;
            }

            case SDL_EVENT_QUIT:
                running = false;
                break;

            default:
                break;
            }
        }

        SDL_PumpEvents();

        const bool* keys = SDL_GetKeyboardState(nullptr);
        constexpr float dt = 0.016f;

        if (keys[SDL_SCANCODE_W]) scene.moveForward(dt);
        if (keys[SDL_SCANCODE_S]) scene.moveBackward(dt);
        if (keys[SDL_SCANCODE_A]) scene.moveLeft(dt);
        if (keys[SDL_SCANCODE_D]) scene.moveRight(dt);
        if (keys[SDL_SCANCODE_SPACE]) scene.moveUp(dt);
        if (keys[SDL_SCANCODE_LCTRL]) scene.moveDown(dt);

        scene.handleMouse(mouse_dx, mouse_dy, dt);

        scene.update();
        scene.render();

        window.swap_buffers();
    }

    SDL_Quit();
    return 0;
}