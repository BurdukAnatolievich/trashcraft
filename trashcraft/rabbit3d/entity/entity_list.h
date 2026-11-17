#pragma once
#include "config.h"

#include <item.h>
#include <inventory.h>
#include <input.h>

#include <ext.hpp>
#include <gtx/norm.hpp>

#include <vector>
#include <algorithm>
#include <mutex>

class c_camera;

class c_item;
class c_block_data;
class c_scene;
class c_scene_object;

class c_entity
{
    c_scene_object* model = nullptr;
public:
    enum e_type
    {
        DEFAULT,
        PLAYER,
        DROPPED_ITEM
    };

    glm::vec3 origin = glm::vec3(0.0f);
    glm::vec3 last_origin = glm::vec3(0.0f);

    glm::ivec2 chunk = glm::ivec2(0);
    glm::ivec2 last_chunk = glm::ivec2(0);

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 last_velocity = glm::vec3(0.0f);

    glm::vec3 eye_position = glm::vec3(0.0f);
    glm::vec3 bb_min = glm::vec3(0.0f), bb_max = glm::vec3(1.0f);

    glm::vec2 view_angle = glm::vec3(0.0f);
    glm::vec2 last_view_angle = glm::vec3(0.0f);

    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 last_rotation = glm::vec3(0.0f);

    glm::vec3 spawn_point = glm::vec3(0.0f, 100.0f, 0.0f);

    float simulation_time = 0.0f;
    float old_simulation_time = 0.0f;

    int type = e_type::DEFAULT;
    bool should_be_destroyed = false;

    c_scene_object* get_model();
    void set_model(c_scene_object* mdl);

    void update_model();

    inline glm::vec3 get_interpolated_abs_origin(float alpha) {
        return glm::mix(last_origin, origin, alpha);
    }

    inline float get_interpolated_yaw(float alpha) {
        float diff = rotation.y - last_rotation.y;
        return last_rotation.y + g_input->normalize_yaw(diff) * alpha;
    }

    inline glm::vec2 get_interpolated_view_angle(float alpha) {
        float yaw_diff = view_angle.y - last_view_angle.y;
        float pitch_diff = view_angle.x - last_view_angle.x;

        return glm::vec2(last_view_angle.x + g_input->normalize_yaw(pitch_diff) * alpha, last_view_angle.y + g_input->normalize_yaw(yaw_diff) * alpha);
    }

    inline void set_origin(const glm::vec3& new_origin) { this->origin = new_origin; };
    inline void set_last_origin(const glm::vec3& new_origin) { this->last_origin = new_origin; };

    inline glm::vec3 get_eye_position() { return origin + eye_position; };

    inline glm::vec3 get_origin() { return origin; };
    inline glm::vec3 get_last_origin() { return last_origin; };

    inline glm::ivec3 get_abs_origin() { return glm::ivec3(static_cast<int>(origin.x) + chunk.x * CHUNK_SIZE, static_cast<int>(origin.y), static_cast<int>(origin.z) + chunk.y * CHUNK_SIZE); };
    inline glm::ivec3 get_last_abs_origin() { return glm::ivec3(static_cast<int>(origin.x) + chunk.x * CHUNK_SIZE, static_cast<int>(origin.y), static_cast<int>(origin.z) + chunk.y * CHUNK_SIZE); };
};

class c_player;

class c_player_animation_state
{
public:
    c_player_animation_state()
    {
        initialized = false;
    }

    c_player_animation_state(c_player* player)
    {
        owner = player;
        initialized = false;
    }

    c_player* owner = nullptr;
    bool initialized = false;

    bool firstperson_swinging = false;
    float firstperson_hand_swing = 0.0f;
    float firstperson_last_hand_swing = 0.0f;

    void reset();
    void update(float eye_yaw, float eye_pitch);
};

class c_player : public c_entity
{
    c_camera* camera = nullptr;
public:
    c_player();
    ~c_player();

    enum e_game_mode
    {
        NONE = -1,
        CREATIVE,
        SURVIVAL,
        MAX
    };

    enum e_damage_reason
    {
        UNKNOWN = 0,
        FALL_DAMAGE,
    };

    struct player_damage_event_t
    {
        int damage = 0;
        e_damage_reason reason = e_damage_reason::UNKNOWN;

        c_player* player = nullptr;
        c_entity* attacker = nullptr;
    };

    struct player_death_event_t
    {
        e_damage_reason reason = e_damage_reason::UNKNOWN;

        c_player* player = nullptr;
        c_entity* attacker = nullptr;
    };

    c_player_animation_state* animation_state = nullptr;

    bool is_alive = false;
    int health = 0, max_health = 20;

    char name[64];

    float next_attack_time = 0.0f;
    float next_interaction_time = 0.0f;

    float attack_progress = 0.0f;
    float interaction_progress = 0.0f;

    struct look_data_t
    {
        uint32_t block = 0;
        glm::ivec3 block_pos = glm::ivec3(0);
        glm::ivec3 block_normal = glm::ivec3(0);

        void reset() {
            block = 0;
            block_pos = block_normal = glm::ivec3(0);
        }
    };

    look_data_t interaction_object;
    look_data_t looking_at;

    accumulated_input_t input;

    bool is_moving = false;
    bool is_sprinting = false;
    bool is_jumping = false;
    bool is_on_ground = false;

    int game_mode = e_game_mode::NONE;

    glm::vec3 fall_origin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);

    c_item* held_item = nullptr;
    c_item* last_held_item = nullptr;

    c_model* held_item_model = nullptr;

	c_inventory inventory = c_inventory();

    void bind_camera(c_camera* camera);
    void unbind_camera();

    inline c_camera* get_bound_camera() { return this->camera; };
    inline c_player_animation_state* get_anim_state() { return this->animation_state; };

	inline c_inventory_item* get_held_item() { return inventory.items[e_inventory_bar::BAR_HOTBAR][g_global_vars->current_inventory_slot].get_ptr(); };

    void respawn();
    void do_damage(int hp, e_damage_reason reason, c_entity* attacker);

    void attack();
    void interact();

    void update_relative_pos();
    void update_looking_at();
    void update_held_item();

    void create_move();

    void update();
};

class c_dropped_item : public c_entity
{
public:
    c_dropped_item()
    {
        type = e_type::DROPPED_ITEM;
    }

    c_inventory_item item;
    bool can_be_picked_up = false;

    void create_model();
    void update();
};

class c_entity_list
{
    std::vector<c_entity*> entities;
public:
    std::mutex entity_mutex;

    void initialize();
    void add_entity(c_entity* entity);
    c_entity* get_entity(int idx);
    int get_entity_count();
    void remove_entity(c_entity* entity);
    void remove_entity(int idx);
    void remove_all_entities();
};

extern c_entity_list* g_entity_list;
