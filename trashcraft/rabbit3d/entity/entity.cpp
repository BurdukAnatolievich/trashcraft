#include "entity_list.h"

#include <global_vars.h>

#include <raytrace/scene.h>
#include <raytrace/render.h>

c_scene_object* c_entity::get_model()
{
    return model;
}

void c_entity::set_model(c_scene_object* new_model)
{
    if (model && model != new_model) {
        g_render->request_free(model);
    }

    c_model* model_ = (c_model*)new_model;
    model = new_model;
}

void c_entity::update_model()
{
    if (!model)
        return;

    model->origin = origin;
    model->rotation = rotation;
}
