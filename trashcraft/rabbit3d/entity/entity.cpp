#include "entity_list.h"

#include <global_vars.h>
#include <world.h>

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

void c_entity::prepare_collision()
{
    target_blocks.clear();
    target_blocks.reserve(36);

    glm::ivec3 round_pos = glm::ivec3(glm::floor(origin));
    for (int y = -1; y <= 2; y++) {
        for (int z = -1; z <= 1; z++) {
            for (int x = -1; x <= 1; x++) {
                glm::ivec3 check_pos = round_pos + glm::ivec3(x, y, z);
                uint32_t block = g_world->get_block_at_local_pos(check_pos) & 0x3FFF;
                if (block) {
                    glm::vec3 block_pos = glm::vec3(check_pos);
                    target_blocks.push_back({
                        block_pos,
                        block_pos + glm::vec3(1.0f, 1.0f, 1.0f)
                        });
                }
            }
        }
    }
}