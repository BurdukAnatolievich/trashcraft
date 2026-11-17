#include "entity_list.h"

#include <raytrace/camera.h>
#include <raytrace/physics.h>
#include <raytrace/models.h>
#include <raytrace/render.h>

#include <time/time.h>

#include <menu.h>
#include <block.h>
#include <global_vars.h>
#include <event_manager.h>
#include <world.h>
#include <iostream>

c_player::c_player()
{
    type = e_type::PLAYER;

    animation_state = new c_player_animation_state(this);
}

c_player::~c_player()
{
    delete animation_state;
    if (held_item_model) {
        g_render->request_free(held_item_model);
    }
}

void c_player::bind_camera(c_camera* camera)
{ 
    if (camera) {
        camera->set_owner(this);
    }

    this->camera = camera; 
}

void c_player::unbind_camera()
{ 
    if (this->camera) {
        this->camera->set_owner(nullptr);
    }
    this->camera = nullptr; 
}

void c_player::do_damage(int hp, e_damage_reason reason, c_entity* attacker)
{
    int last_health = health;
    health -= hp;

    player_damage_event_t damage_event_data = { hp, reason, this, attacker };
    g_event_manager->handle_listeners("on_player_damage", &damage_event_data);

    if (last_health > 0 && health <= 0)
    {
        player_death_event_t death_event_data = { reason, this, attacker };
        g_event_manager->handle_listeners("on_player_death", &death_event_data);
    }
}

void c_player::respawn()
{
    delete animation_state;
    animation_state = new c_player_animation_state(this);

    fall_origin = glm::vec3(FLT_MAX);

    set_origin(spawn_point);
    set_last_origin(spawn_point);

    health = 20;
    is_alive = true;

    next_attack_time = next_interaction_time = 0.0f;

    chunk = glm::ivec2(0);

    velocity = last_velocity = glm::vec3(0.0f);
    //chunk_origin = last_chunk_origin = glm::ivec3(0);

    view_angle = last_view_angle = glm::vec2(0.0f);
    rotation = last_rotation = glm::vec3(0.0f);
}

void c_player::attack()
{
    if (input.buttons & e_buttons::IN_ACTION) {
        return;
    }

    if (g_input->get_is_locked()) {
        return;
    }

    c_inventory_item* inv_item = &g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][g_global_vars->current_inventory_slot];
    if (!inv_item) {
        return;
    }

    c_item* hotbar_item = inv_item->item_data.item;

    if (input.buttons & IN_ATTACK) {
        if (simulation_time < next_attack_time) {
            return;
        }

        if (!looking_at.block || interaction_object.block_pos != looking_at.block_pos) {
            attack_progress = 0.0f;
            interaction_object.reset();
        }

        if (looking_at.block) {
            uint16_t block_id = looking_at.block & 0x3FFF;
            if (!block_id) {
                return;
            }

            c_block* target_block = static_cast<c_block*>(g_global_vars->mapped_items[block_id]);
            if (!target_block) {
                return;
            }

            if (target_block->hardness < 0.0f || target_block->mining_tool == c_tool::e_tool::NONE) {
                attack_progress = 0.0f;
                interaction_object.reset();
                return;
            }

            c_tool* current_tool = nullptr;
            if (hotbar_item && hotbar_item->type == c_item::e_type::TOOL) {
                current_tool = static_cast<c_tool*>(hotbar_item);
            }

            bool is_effective_tool = current_tool && (current_tool->tool & target_block->mining_tool);
            bool can_harvest = (target_block->mining_tool & c_tool::e_tool::ANY) || is_effective_tool;

            float efficiency = is_effective_tool ? current_tool->efficiency : 1.0f;
            float break_time_in_seconds = can_harvest
                ? (target_block->hardness * 1.5f) / efficiency
                : (target_block->hardness * 5.0f);

            float dt = g_time->get_interval_per_tick();
            interaction_object = looking_at;

            if (break_time_in_seconds <= 0.0f) {
                attack_progress = 1.0f;
            }
            else {
                attack_progress += dt / break_time_in_seconds;
            }

            if (attack_progress >= 1.0f) {
                g_world->remove_block(looking_at.block_pos);

                attack_progress = 1.0f;
                next_attack_time = simulation_time + (5.0f * dt);

                interaction_object.reset();
            }
        }
    }
    else {
        attack_progress = 0.0f;
        next_attack_time = 0.0f;
        interaction_object.reset();
    }
}

void c_player::interact()
{
    if (input.buttons & e_buttons::IN_ATTACK) {
        return;
    }

    if (g_input->get_is_locked()) {
        return;
    }

    c_inventory_item* inv_item = &g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][g_global_vars->current_inventory_slot];
    if (!inv_item) {
        return;
    }

    c_item* hotbar_item = inv_item->item_data.item;

    float cur_time = g_time->get_current_time();
    if (input.buttons & IN_ACTION) {
        if (!hotbar_item || hotbar_item->type != c_item::e_type::BLOCK) {
            return;
        }

        if (interaction_progress >= 1.0f) {
            return;
        }
        
        c_block* block = (c_block*)hotbar_item;
        if (looking_at.block) {
            uint16_t block_id = looking_at.block & 0x3FFF;
            if (!block_id) {
                return;
            }

            g_world->place_block(block, looking_at.block_pos + looking_at.block_normal);
            interaction_progress = 1.0f;
        }
    }
    else {
        interaction_progress = 0.0f;
    }
}

void c_player::update_relative_pos()
{
    glm::ivec2 chunk_shift = {
        static_cast<int>(std::floor(origin.x)) >> CHUNK_SIZE_SHIFT,
        static_cast<int>(std::floor(origin.z)) >> CHUNK_SIZE_SHIFT
    };

    if (chunk_shift != glm::ivec2(0)) {
        constexpr int chunk_size = 1 << CHUNK_SIZE_SHIFT;
        glm::vec3 offset = glm::vec3(chunk_shift.x * chunk_size, 0.0f, chunk_shift.y * chunk_size);

        chunk += chunk_shift;

        last_origin -= offset;
        origin -= offset;
    }
}

void c_player::update_looking_at()
{
    glm::vec3 eye_position = get_eye_position();

    trace_t trace;
    g_raytrace_utils->voxel_raycast(eye_position, camera->get_forward(), trace, 5);

    if (trace.hit) {
        looking_at.block = trace.block;
        looking_at.block_pos = trace.hit_pos;
        looking_at.block_normal = trace.normal;
    }
    else {
        looking_at.block = 0;
        looking_at.block_pos = glm::ivec3(0);
        looking_at.block_normal = glm::ivec3(0);
    }
}

void c_player::update_held_item()
{
    held_item = g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][g_global_vars->current_inventory_slot].item_data.item;

    if (held_item != last_held_item) {
        if (held_item_model) {
            g_render->request_free(held_item_model);
            held_item_model = nullptr;
        }

        if (held_item) {
            held_item_model = g_model_manager->get_model_instance(held_item->item_model);
        }

        last_held_item = held_item;
    }
}

void c_player::create_move()
{
    g_input->get_accumulated_input(input);

    view_angle += input.angle_change;
    view_angle = g_input->clamp_angles(view_angle);

    rotation.y = view_angle.y;

    bool moving_forward = input.buttons & e_buttons::IN_MOVE_FORWARD;
    bool moving_backward = input.buttons & e_buttons::IN_MOVE_BACKWARD;
    bool moving_right = input.buttons & e_buttons::IN_MOVE_RIGHT;
    bool moving_left = input.buttons & e_buttons::IN_MOVE_LEFT;

    bool sprint_pressed = input.buttons & e_buttons::IN_SPRINT;
    if (sprint_pressed && moving_forward && !moving_backward) {
        is_sprinting = true;
    }

    if (!moving_forward || moving_backward) {
        is_sprinting = false;
    }

    float move_yaw = 0.0f;
    if (moving_right || moving_left)
    {
        if (!moving_forward && !moving_backward)
            move_yaw = view_angle.y + (moving_right ? 90.0f : -90.0f);
        else
        {
            if (moving_forward)
                move_yaw = view_angle.y + (moving_right ? 45.0f : -45.0f);
            else if (moving_backward)
                move_yaw = view_angle.y + (moving_right ? 135.0f : -135.0f);
        }
    }
    else
    {
        if (moving_forward)
            move_yaw = view_angle.y;
        if (moving_backward)
            move_yaw = view_angle.y + 180.0f;
    }

    float yaw_radians = glm::radians(move_yaw);

    glm::vec3 wish_dir = glm::vec3(-sin(yaw_radians), 0.0f, cos(yaw_radians));
    if (glm::length(wish_dir) > 0.0f) {
        wish_dir = glm::normalize(wish_dir);
    }

    if (!moving_forward && !moving_backward && !moving_right && !moving_left)
        wish_dir = glm::vec3(0.0f);

    if (!is_on_ground) {
        velocity.y -= 0.08f;
        if (velocity.y < -3.92f)
            velocity.y = -3.92f;
    }
    else {
        if (velocity.y < 0.0f)
            velocity.y = 0.0f;
    }

    if ((input.buttons & e_buttons::IN_JUMP) && is_on_ground) {
        velocity.y = 0.42f;
        is_on_ground = false;

        if (is_sprinting) {
            velocity += wish_dir * 0.2f;
        }
    }

    float friction = is_on_ground ? 0.546f : 0.91f;

    float base_accel = is_on_ground ? 0.1f : 0.02f;
    float acceleration_factor = is_sprinting ? (base_accel * 1.3f) : base_accel;

    velocity.x += wish_dir.x * acceleration_factor;
    velocity.z += wish_dir.z * acceleration_factor;

    bool stood_on_ground = false;
    bool collided_horizontally = false; 

    glm::ivec3 round_pos = glm::ivec3(glm::floor(origin));
    glm::vec3 current_pos = origin;

    std::vector<aabb_t> target_blocks;
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

    const float half_width = 0.3f;
    const float player_height = 1.8f;
    const float eps = 0.00001f;

    auto get_current_player_box = [&](const glm::vec3& pos) -> aabb_t {
        return {
            pos + glm::vec3(-half_width, 0.0f, -half_width),
            pos + glm::vec3(half_width, player_height, half_width)
        };
        };

    current_pos.x += velocity.x;
    aabb_t player_box = get_current_player_box(current_pos);
    for (const auto& block_box : target_blocks) {
        if (g_physics->check_aabb_collision(player_box, block_box)) {
            if (velocity.x > 0.0f) current_pos.x = block_box.min.x - half_width - eps;
            else if (velocity.x < 0.0f) current_pos.x = block_box.max.x + half_width + eps;
            velocity.x = 0.0f;
            collided_horizontally = true;
            player_box = get_current_player_box(current_pos);
        }
    }

    current_pos.z += velocity.z;
    player_box = get_current_player_box(current_pos);
    for (const auto& block_box : target_blocks) {
        if (g_physics->check_aabb_collision(player_box, block_box)) {
            if (velocity.z > 0.0f) current_pos.z = block_box.min.z - half_width - eps;
            else if (velocity.z < 0.0f) current_pos.z = block_box.max.z + half_width + eps;
            velocity.z = 0.0f;
            collided_horizontally = true;
            player_box = get_current_player_box(current_pos);
        }
    }

    current_pos.y += velocity.y;
    player_box = get_current_player_box(current_pos);
    for (const auto& block_box : target_blocks) {
        if (g_physics->check_aabb_collision(player_box, block_box)) {
            if (velocity.y > 0.0f) {
                current_pos.y = block_box.min.y - player_height - eps;
                velocity.y = 0.0f;
            }
            else if (velocity.y < 0.0f) {
                current_pos.y = block_box.max.y + eps;
                velocity.y = 0.0f;
                stood_on_ground = true;
            }
            player_box = get_current_player_box(current_pos);
        }
    }

    if (collided_horizontally) {
        is_sprinting = false;
    }

    is_on_ground = stood_on_ground;
    origin = current_pos;

    velocity.x *= friction;
    velocity.z *= friction;
    velocity.y *= 0.98f;
}

void c_player::update()
{
    old_simulation_time = simulation_time;
    simulation_time = g_time->get_current_time();

    input.buttons = 0;
    input.angle_change = glm::vec2(0.0f);

    if (health < 0)
        health = 0;

    is_alive = health > 0;

    if (!is_alive) {
        set_model(nullptr);
        
        if (held_item_model) {
            g_render->request_free(held_item_model);
        }
        return;
    }

    last_origin = origin;
    last_velocity = velocity;
    last_rotation = rotation;
    last_view_angle = view_angle;

    update_held_item();
    update_looking_at();

    create_move();
    update_relative_pos();

    attack();
    interact();

    update_model();

    //animation_state->update(view_angle.y, view_angle.x);

    eye_position = glm::vec3(0.0f, 1.7f, 0.0f);
}