#pragma once
#include <map>
#include <string>
#include <unordered_map>

#include "item.h"
#include <glm.hpp>

class c_player;
class c_camera;
class c_block;
class c_item;
class c_scene;
class c_inventory_item;
class c_crafting_recipe;
class c_scene_object;

enum e_inventory_bar
{
    BAR_INVALID = -1,
    BAR_1,
    BAR_2,
    BAR_3,
    BAR_HOTBAR,
    BAR_MAX
};

class c_global_vars
{
public:
    c_player* local_player = nullptr;

    c_scene* scene = nullptr;
    c_camera* camera = nullptr;

    int current_inventory_slot = 0;

    std::map<std::string, c_item*> items;
    std::map<std::string, c_crafting_recipe*> recipes;

    c_item* mapped_items[16384] = { nullptr };
    bool mapped_transparency[16384] = { false };

    c_inventory_item inventory_items[4][9];
    c_inventory_item* ignore_inventory_slot = nullptr;

    c_crafting_recipe* crafting_recipe = nullptr;
    c_crafting_recipe::e_grid_type crafting_grid = c_crafting_recipe::e_grid_type::GRID_2X2;
    c_inventory_item crafting_grid_items[3][4];

    bool in_game = false;
    bool draw_hud = true;

    int world_id = -1;
    std::string world_name = "";

    void initialize_blocks();
    void initialize_items();

    void initialize_recipes();

    c_inventory_item* give_item(c_item* item, int count = 1);
    void drop_item(c_inventory_item* item, glm::vec3 origin = glm::vec3(FLT_MAX), glm::vec3 up = glm::vec3(FLT_MAX));

    bool put_item(c_inventory_item* item);

    void start();
    void finish();

    void respawn();

    void map_items();
};

extern c_global_vars* g_global_vars;
