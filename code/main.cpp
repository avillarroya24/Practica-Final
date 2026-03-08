#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Scene.hpp"
#include "Skybox.hpp"
#include "Camera.hpp"

using namespace udit;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Sistema Solar", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    Scene scene(1280, 720);
    Camera camera;
    Skybox skybox("assets/skybox/");

    while (!glfwWindowShouldClose(window))
    {
        scene.update();

        skybox.render(camera);
        scene.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
