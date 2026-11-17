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

    bool in_game = false;

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
