#include "Scene.hpp"

Scene::Scene()
{
    cube_mesh = make_shared<Cube>();

    material = make_shared<Material>();

    cube1.mesh = cube_mesh;
    cube1.material = material;
    cube1.transform.position = glm::vec3(0, 0, -10);
    cube1.transform.scale = glm::vec3(2, 2, 2);

    cube2.mesh = cube_mesh;
    cube2.material = material;
    cube2.transform.position = glm::vec3(3, 0, 0);
    cube2.transform.scale = glm::vec3(0.5, 0.5, 0.5);

    cube2.set_parent(cube1);
}

void Scene::update(float time)
{
    cube1.transform.rotation.y = time;
}

void Scene::render()
{
    cube1.render(model_view_matrix_id);
    cube2.render(model_view_matrix_id);
}