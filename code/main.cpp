#include "Scene.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

using namespace udit;

int main(int, char* [])
{
    constexpr unsigned viewport_width = 1024;
    constexpr unsigned viewport_height = 576;

    Window window("OpenGL example", viewport_width, viewport_height, { 3, 3 });
    Scene  scene(viewport_width, viewport_height);

    Camera camera(60.f, 0.1f, 100.f, float(viewport_width)/float(viewport_height));
    float speed = 0.1f;

    bool exit = false;
    int lastMouseX = viewport_width / 2;
    int lastMouseY = viewport_height / 2;
    SDL_SetRelativeMouseMode(SDL_TRUE); // captura el ratón

    do
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                exit = true;

            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                int x = event.motion.xrel;
                int y = event.motion.yrel;

                float sensitivity = 0.002f;
                camera.rotate(float(x) * sensitivity, float(-y) * sensitivity);
            }
        }

        // --- Procesar teclado ---
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);

        glm::vec3 forward = glm::normalize(glm::vec3(camera.get_target() - camera.get_location()));
        glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0.f,1.f,0.f)));

        if (keystate[SDL_SCANCODE_W]) camera.move(forward * speed);
        if (keystate[SDL_SCANCODE_S]) camera.move(-forward * speed);
        if (keystate[SDL_SCANCODE_A]) camera.move(-right * speed);
        if (keystate[SDL_SCANCODE_D]) camera.move(right * speed);
        if (keystate[SDL_SCANCODE_SPACE]) camera.move(glm::vec3(0.f, speed, 0.f));
        if (keystate[SDL_SCANCODE_LSHIFT]) camera.move(glm::vec3(0.f, -speed, 0.f));

        // --- Actualizar y renderizar escena ---
        scene.update();

        // Antes de renderizar, sube las matrices de cámara a tus shaders
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 projection = camera.get_projection_matrix();
        scene.set_camera_matrices(view, projection); // <-- añade este método a Scene.hpp si no existe

        scene.render();
        window.swap_buffers();

    } while (!exit);

    SDL_Quit();
    return 0;
}