#pragma once
#include <raytrace/scene.h>

#include "global_vars.h"
#include "item.h"

struct world_chunk_t;

class c_block : public c_item
{
public:
    enum e_material
    {
        SIDE,
        UPPER,
        BOTTOM,
        MAX
    };

    enum e_block_type
    {
        T_UNKNOWN = -1,
        T_DEFAULT,
        T_CHEST,
        T_DOOR,
        T_JUKEBOX,
    };

    enum e_interactable_type
    {
        I_NONE,
        I_CRAFTING_TABLE,
        I_JUKEBOX,
        I_CHEST,
        I_DOOR,
    };

    int block_type = e_block_type::T_UNKNOWN;

    c_texture* materials[e_material::MAX];

    float hardness = 0.0f;
    int mining_tool = 0, mining_tool_to_drop = 0;

    c_item* drop_item = nullptr;

    glm::vec3 min = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 max = glm::vec3(1.0f, 1.0f, 1.0f);

    int interactable = e_interactable_type::I_NONE;
    bool is_transparent = false, is_solid = false, is_light = false;

    c_block(std::string register_name, std::string register_id, std::vector<std::string> material_array, glm::vec3 mins, glm::vec3 maxs, float hardness = 1.0f, int tool = -1, int tool_to_drop = -1, std::string drop_item = "", bool is_solid = true, bool is_transparent = false, bool is_light = false, int block_type = e_block_type::T_DEFAULT, std::string hotbar_texture = "", int interactable = e_interactable_type::I_NONE, std::string model = "none");

    c_scene_object* create_scene_object(const glm::vec3& origin, float scale = 1.0f);
};
//
//class c_block_data
//{
//
//};
//
//class c_jukebox : public c_block_data
//{
//public:
//    c_jukebox(c_block* block_)
//    {
//        block = block_;
//    }
//
//    bool has_disc = false;
//    c_inventory_item disc;
//
//    int current_channel = -1;
//};
//
//class c_chest : public c_block_data
//{
//public:
//    c_chest(c_block* block_)
//    {
//        block = block_;
//
//        std::memset(items, 0, sizeof(c_inventory_item) * 3 * 9);
//    }
//
//    bool is_opened = false;
//    c_inventory_item items[3][9];
//};
//
//class c_door : public c_block_data
//{
//public:
//    c_door(c_block* block_)
//    {
//        block = block_;
//    }
//
//    bool is_opened = false;
//};
