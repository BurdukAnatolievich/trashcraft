#include "scene.h"
#include "camera.h"
#include "skybox.h"

#include "entity/entity_list.h"

#include <global_vars.h>
#include <config.h>

c_skybox* c_scene::get_skybox()
{
    return skybox;
}

void c_scene::set_skybox(c_skybox* sky)
{
    skybox = sky;
}