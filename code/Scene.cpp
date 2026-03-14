#include "Scene.hpp"
#include <windows.h>
#include <GL/gl.h>

Scene::Scene()
{
    cube1.transform.position[0] = 0.0f;
    cube1.transform.position[1] = 0.0f;
    cube1.transform.position[2] = -10.0f;

    cube2.transform.position[0] = 4.0f;
    cube2.transform.position[1] = 0.0f;
    cube2.transform.position[2] = -10.0f;
}

void Scene::update(float time)
{
    cube1.transform.rotation[1] = time;
    cube2.transform.rotation[1] = time * 3.0f;
}

void Scene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cube1.render();
    cube2.render();
}