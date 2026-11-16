#include "skybox.h"
#include "render.h"

void c_skybox::update()
{

}

glm::vec3 c_skybox::get_sun_position()
{
    return sun_position;
}

void c_skybox::set_sun_position(const glm::vec3& position)
{
    sun_position = position;
}
