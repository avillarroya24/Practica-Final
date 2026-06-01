#include "Scene.hpp"
#include "Window.hpp"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

/*

    Punto de entrada de la aplicación.
 
 * Este archivo contiene la función principal del programa. Se encarga de:
 * - Crear la ventana y la escena
 * - Gestionar eventos de entrada (teclado y ratón)
 * - Ejecutar el bucle principal (update + render)
 * - Finalizar correctamente SDL
*/

using udit::Scene;
using udit::Window;

int main(int, char* [])
{
    constexpr unsigned WIDTH = 1024; //Ancho de la ventana
    constexpr unsigned HEIGHT = 576; //Alto de la ventana

    Window window("OpenGL example", WIDTH, HEIGHT, { 3, 3 }); //Ventana principal
    Scene scene(WIDTH, HEIGHT); //Escena 3D

    bool running = true; //Controla el bucle principal
    bool first_mouse = true; //Control para inicializar el raton

    float mouse_x = 0.0f, mouse_y = 0.0f; //Posicion actual del raton
    float last_mouse_x = WIDTH * 0.5f; //Ultima posicion X del raton
    float last_mouse_y = HEIGHT * 0.5f; //Ultima posicion Y del raton

    while (running)
    {
        SDL_Event event;

        float mouse_dx = 0.0f; //Movimiento en X del raton
        float mouse_dy = 0.0f; //Movimiento en Y del raton

        // ================= EVENTOS =================

        /*
         * Procesa todos los eventos del sistema:
         * - Movimiento del ratón
         * - Cierre de la ventana
        */

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

                mouse_dx += (mouse_x - last_mouse_x); //Calculo del desplazamiento del raton
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

        // ================= INPUT TECLADO =================

        //Consulta el estado del teclado para mover la camara

        const bool* keys = SDL_GetKeyboardState(nullptr);
        constexpr float dt = 0.016f;

        if (keys[SDL_SCANCODE_W]) scene.moveForward(dt);
        if (keys[SDL_SCANCODE_S]) scene.moveBackward(dt);
        if (keys[SDL_SCANCODE_A]) scene.moveLeft(dt);
        if (keys[SDL_SCANCODE_D]) scene.moveRight(dt);
        if (keys[SDL_SCANCODE_SPACE]) scene.moveUp(dt);
        if (keys[SDL_SCANCODE_LCTRL]) scene.moveDown(dt);

        scene.handleMouse(mouse_dx, mouse_dy, dt); //Aplica la rotacion de camara en funcion del movimiento del raton

        scene.update(); //Actualiza la logica de la escena
        scene.render(); //Dibuja la escena en pantalla

        window.swap_buffers(); //Intercambia buffers (doble buffer)
    }

    SDL_Quit(); //Finaliza SDL correctamente
    return 0;
}