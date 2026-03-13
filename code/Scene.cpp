#include "Scene.hpp"

Scene::Scene()
{
    cube_mesh = std::make_shared<Cube>();

    material = std::make_shared<Material>();
    material->shader_program =
        std::make_shared<Shader_Program>("vertex shader", "fragment shader");

    model_view_matrix_id =
        material->shader_program->get_uniform_location("model_view_matrix");

    projection_matrix_id =
        material->shader_program->get_uniform_location("projection_matrix");

    cube1.mesh = cube_mesh;
    cube1.material = material;
    cube1.transform.position = glm::vec3(0, 0, -10);
    cube1.transform.scale = glm::vec3(2, 2, 2);

    cube2.mesh = cube_mesh;
    cube2.material = material;
    cube2.transform.position = glm::vec3(3, 0, 0);

    cube2.set_parent(cube1);
}

void Scene::update(float time)
{
    // cubo grande gira sobre sí mismo
    cube1.transform.rotation.y = time;

    // cubo pequeño gira sobre sí mismo
    cube2.transform.rotation.y = time * 3;
}

void Scene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cube1.render(model_view_matrix_id);
    cube2.render(model_view_matrix_id);
}
