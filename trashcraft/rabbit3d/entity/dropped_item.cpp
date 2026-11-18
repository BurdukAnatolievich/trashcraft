#include "entity_list.h"

#include <block.h>
#include <time/time.h>

#include <raytrace/scene.h>
#include <raytrace/physics.h>
#include <raytrace/models.h>

void c_dropped_item::create_model()
{
    if (get_model())
        return;


}

void c_dropped_item::update()
{
    if (should_be_destroyed)
        return;

    old_simulation_time = simulation_time;
    simulation_time = g_time->get_current_time();

    glm::vec3 current_origin = get_origin();

    // rotate 45 degrees per second
    rotation.y += 45.0f * g_time->get_interval_per_tick();

    last_velocity = velocity;


    c_player* local = g_global_vars->local_player;
    //if (local)
    //{
    //    if (can_be_picked_up)
    //    {
    //        glm::vec3 local_origin = local->get_origin();
    //        if (glm::length2(local_origin - current_origin) < 1.0f) {
    //            g_global_vars->put_item(&item);
    //        }
    //    }
    //}

    set_origin(current_origin + velocity);

    create_model();
    update_model();

    if (item.is_empty())
        should_be_destroyed = true;
}
