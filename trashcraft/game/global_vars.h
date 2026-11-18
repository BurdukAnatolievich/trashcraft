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

    bool in_game = false;
    bool cheats_allowed = false;

    std::map<std::string, c_item*> items;
    std::map<std::string, c_crafting_recipe*> recipes;

    std::unordered_map<std::string, int> game_rules;

    c_item* mapped_items[MAX_ITEM_COUNT] = { nullptr };
    bool mapped_transparency[MAX_ITEM_COUNT] = { false };

    int world_id = -1;
    std::string world_name = "";

    void initialize_blocks();
    void initialize_items();
    void initialize_recipes();

    inline bool get_game_rule(const std::string& rule, int& rule_value) { auto iterator = game_rules.find(rule); if (iterator == game_rules.end()) return false; rule_value = iterator->second; return true; }
    inline bool set_game_rule(const std::string& rule, int rule_value) { auto iterator = game_rules.find(rule); if (iterator == game_rules.end()) return false; iterator->second = rule_value; return true; }

    void start();
    void finish();

    void respawn();
    void map_items();
};

extern c_global_vars* g_global_vars;
