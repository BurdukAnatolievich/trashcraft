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
    //elocity = g_physics->correct_movement_simplified(this, velocity, last_velocity);

    //velocity.x = g_math->Lerp(velocity.x, 0.0f, g_time->get_interval_per_tick() * 2.0f);
    //velocity.y = g_math->Lerp(velocity.y, 0.0f, g_time->get_interval_per_tick() * 2.0f);

    //if (velocity.Length2D() < 0.002f)
    //{
    //    velocity.x = velocity.y = 0.0f;
    //    can_be_picked_up = true;
    //}

    //bool local_player_picking_up = picking_up && picking_up_entity == g_global_vars->local_player;
    //if (g_global_vars->local_player && (velocity.IsZero() || local_player_picking_up))
    //{
    //    c_player* local = g_global_vars->local_player;

    //    Vector local_origin = local->get_origin();
    //    float distance_to_local = (local_origin - current_origin).Length2D();
    //    if (picking_up || distance_to_local < 1.0f)
    //    {
    //        Vector new_velocity = local->eye_position - current_origin;
    //        velocity = g_math->Lerp(velocity, new_velocity, g_time->get_interval_per_tick() * 8.0f);

    //        picking_up = true;
    //        picking_up_entity = g_global_vars->local_player;
    //    }

    //    if (local_player_picking_up && velocity.Length() < 0.05f && last_velocity.Length() > velocity.Length())
    //    {


    //        picking_up = false;
    //        should_be_destroyed = true;
    //    }
    //}

    c_player* local = g_global_vars->local_player;
    if (local)
    {
        if (can_be_picked_up)
        {
            glm::vec3 local_origin = local->get_origin();
            if (glm::length2(local_origin - current_origin) < 1.0f) {
                g_global_vars->put_item(&item);
            }
        }
    }

    set_origin(current_origin + velocity);

    create_model();
    update_model();

    if (item.is_empty())
        should_be_destroyed = true;
}
