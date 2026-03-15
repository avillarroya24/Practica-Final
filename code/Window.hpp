#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <utility>

namespace udit
{

    class Window
    {
    public:

        struct OpenGL_Context_Settings
        {
            unsigned version_major = 3;
            unsigned version_minor = 3;
            bool     core_profile = true;
            unsigned depth_buffer_size = 24;
            unsigned stencil_buffer_size = 0;
            bool     enable_vsync = true;
        };

    private:

        SDL_Window* window_handle = nullptr;
        SDL_GLContext opengl_context = nullptr;

    public:

        // Constructor con std::string
        Window(const std::string& title,
            unsigned width,
            unsigned height,
            const OpenGL_Context_Settings& context_details)
            : Window(title.c_str(), width, height, context_details)
        {
        }

        // Constructor principal
        Window(const char* title,
            unsigned width,
            unsigned height,
            const OpenGL_Context_Settings& context_details);

        ~Window();

    public:

        // No copiar
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        // Move constructor
        Window(Window&& other) noexcept
        {
            window_handle = std::exchange(other.window_handle, nullptr);
            opengl_context = std::exchange(other.opengl_context, nullptr);
        }

        // Move assignment
        Window& operator=(Window&& other) noexcept
        {
            if (this != &other)
            {
                window_handle = std::exchange(other.window_handle, nullptr);
                opengl_context = std::exchange(other.opengl_context, nullptr);
            }

            return *this;
        }

    public:

        void swap_buffers();
    };

}