#include "Scene.hpp"
#include "Window.hpp"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

using udit::Scene;
using udit::Window;

int main(int, char* [])
{
    constexpr unsigned viewport_width = 1024;
    constexpr unsigned viewport_height = 576;

    Window window("OpenGL example", viewport_width, viewport_height, { 3, 3 });
    Scene scene(viewport_width, viewport_height);

    bool exit = false;

    float mouse_x = 0.0f;
    float mouse_y = 0.0f;

    float last_mouse_x = viewport_width * 0.5f;
    float last_mouse_y = viewport_height * 0.5f;

    bool first_mouse = true;

    do
    {
        SDL_Event event;

        float mouse_dx = 0.0f;
        float mouse_dy = 0.0f;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                SDL_GetMouseState(&mouse_x, &mouse_y);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                SDL_GetMouseState(&mouse_x, &mouse_y);
                break;
            }

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
            {
                exit = true;
                break;
            }
            }
        }

        // =========================
        // INPUT TECLADO (WASD)
        // =========================
        SDL_PumpEvents();
        const bool* keys = SDL_GetKeyboardState(nullptr);

        float dt = 0.016f;

        if (keys[SDL_SCANCODE_W])
            scene.moveForward(dt);
        if (keys[SDL_SCANCODE_S])
            scene.moveBackward(dt);
        if (keys[SDL_SCANCODE_A])
            scene.moveLeft(dt);
        if (keys[SDL_SCANCODE_D])
            scene.moveRight(dt);
        if (keys[SDL_SCANCODE_SPACE])
            scene.moveUp(dt);
        if (keys[SDL_SCANCODE_LCTRL])
            scene.moveDown(dt);

        // =========================
        // MOUSE LOOK
        // =========================
        scene.handleMouse(mouse_dx, mouse_dy, dt);

        // =========================
        // UPDATE + RENDER
        // =========================
        scene.update();
        scene.render();

        window.swap_buffers();

    } while (!exit);

    SDL_Quit();
    return 0;
}