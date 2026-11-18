#pragma once
#include <string>
#include <deque>
#include <unordered_map>

#include "config.h"

class c_world;
class c_item;
struct world_chunk_t;

class c_world_save
{
public:
    struct world_info_t
    {
        std::string display_name = "";
        std::string actual_name = "";

        uint64_t last_played = 0;

        int game_mode = 0;
        bool cheats_allowed = false;

        std::unordered_map<std::string, int> game_rules;
    };

#pragma pack(push, 1)
    struct saved_item_entry_t
    {
        uint16_t id = 0;
        char name[64] = { 0 };
    };
#pragma pack(pop)
private:
    std::string saves_path = "saves/";
public:
    std::deque<world_info_t> worlds;

    std::unordered_map<uint16_t, c_item*> id_to_item;
    std::unordered_map<c_item*, uint16_t> item_to_id;

    void initialize();

    void load_world(int id);
    void save_world();

    bool load_chunk(const std::string& chunk_id);
    bool save_chunk(world_chunk_t* chunk);

    void create_world(const std::string& display_name, const std::string& actual_name);
};

extern c_world_save* g_world_save;
