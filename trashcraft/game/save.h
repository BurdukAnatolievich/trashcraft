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

        std::string chunks_path = "";

        std::vector<std::string> chunks;
        std::vector<std::string> parsed_info;
    };

    struct save_chunk_t
    {
        glm::ivec2 index = glm::ivec2(0, 0);
        uint32_t blocks[CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT];
    };
private:
    std::string saves_path = "saves/";
public:
    std::deque<world_info_t> worlds;

    std::unordered_map<uint16_t, c_item*> indices_to_item;
    std::unordered_map<c_item*, uint16_t> item_to_indices;

    void initialize();

    void load_world(int id);
    void save_world();

    void load_chunk(world_chunk_t* chunk);
    void load_chunk(std::string chunk);
    void save_chunk(world_chunk_t* chunk);

    void create_world(const std::string& display_name, const std::string& actual_name);
};

extern c_world_save* g_world_save;
