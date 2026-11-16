#pragma once
#include <glm.hpp>

class c_skybox
{
    glm::vec3 sun_position = glm::vec3(0.0f);
public:
    c_skybox()
    {

    }

    void update();

    glm::vec3 get_sun_position();
    void set_sun_position(const glm::vec3& position);
};