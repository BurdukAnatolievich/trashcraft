#include "save.h"
#include "world.h"
#include "block.h"
#include "global_vars.h"

#include <entity/entity_list.h>
#include <raytrace/models.h>

#include <filesystem>
#include <fstream>
#include <sstream>

c_world_save* g_world_save = new c_world_save;

void c_world_save::initialize()
{
    std::filesystem::create_directory(saves_path);

    for (const auto& entry : std::filesystem::directory_iterator(saves_path))
    {
        if (!entry.is_directory())
            continue;

        std::filesystem::path world_info_path = entry.path().string() + "/world_info.tcworld";
        std::ifstream world_info(world_info_path.string(), std::ios::binary);
        if (world_info.is_open())
        {
            
            world_info.close();
        }
    }
}

void c_world_save::load_world(int id)
{
    if (id < 0 || id >= static_cast<int>(worlds.size())) {
        return;
    }

    world_info_t& info = worlds.at(id);

    g_world->name = info.actual_name;
    g_world->info = &info;
    g_global_vars->world_id = id;
    g_global_vars->world_name = info.display_name;

    id_to_item.clear();

    std::string info_path = saves_path + g_world->name + "/world_info.tcworld";
    std::ifstream file(info_path, std::ios::binary);

    if (file.is_open()) {
        char magic[14] = { 0 };
        file.read(magic, sizeof(magic));

        if (file.gcount() == sizeof(magic) && std::memcmp(magic, "TrashcraftSave", 14) == 0) {
            int32_t gen_version = 0;
            file.read((char*)&gen_version, sizeof(gen_version));

            if (gen_version != TRASHCRAFT_WORLD_GENERATOR_VERSION) {
                file.close();
                return;
            }

            char display_name_buf[64] = { 0 };
            file.read(display_name_buf, sizeof(display_name_buf));
            display_name_buf[63] = '\0';
            info.display_name = display_name_buf;
            g_global_vars->world_name = info.display_name;

            file.read((char*)&info.last_played, sizeof(uint64_t));

            int32_t game_mode = 0;
            file.read((char*)&game_mode, sizeof(int32_t));
            info.game_mode = game_mode;

            uint8_t cheats = 0;
            file.read((char*)&cheats, sizeof(uint8_t));
            info.cheats_allowed = (cheats != 0);
            g_global_vars->cheats_allowed = info.cheats_allowed;

            glm::vec3 spawn_origin(0.0f); glm::ivec2 spawn_chunk(0);
            file.read((char*)&spawn_origin, sizeof(glm::vec3));
            file.read((char*)&spawn_chunk, sizeof(glm::ivec2));
            if (g_global_vars->local_player) {
                g_global_vars->local_player->origin = spawn_origin;
                g_global_vars->local_player->chunk = spawn_chunk;
            }

            uint16_t rules_count = 0;
            file.read((char*)&rules_count, sizeof(uint16_t));
            for (uint16_t i = 0; i < rules_count; ++i) {
                char rule_buf[64] = { 0 };
                int32_t val = 0;
                file.read(rule_buf, sizeof(rule_buf));
                file.read((char*)&val, sizeof(int32_t));
                rule_buf[63] = '\0';

                g_global_vars->set_game_rule(rule_buf, val);
            }

            uint16_t items_count = 0;
            file.read((char*)&items_count, sizeof(uint16_t));

            for (uint16_t i = 0; i < items_count; ++i) {
                saved_item_entry_t entry{};
                file.read((char*)&entry, sizeof(saved_item_entry_t));
                entry.name[63] = '\0';

                auto it = g_global_vars->items.find(entry.name);
                if (it != g_global_vars->items.end()) {
                    id_to_item[entry.id] = it->second;
                }
            }
        }
        file.close();
    }

    std::string chunks_dir = saves_path + g_world->name + "/chunks/";
    if (std::filesystem::exists(chunks_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(chunks_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".tchunk") {
                load_chunk(entry.path().stem().string());
            }
        }
    }

    g_world->build_chunks();

    id_to_item.clear();
}

void c_world_save::save_world()
{
    if (!g_world || !g_world->info) {
        return;
    }

    std::string world_dir = saves_path + g_world->name + "/";
    std::filesystem::create_directories(world_dir + "chunks/");

    g_world->info->last_played = static_cast<uint64_t>(std::time(nullptr));

    std::ofstream file(world_dir + "world_info.tcworld", std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return;
    }

    const char magic[14] = { 'T','r','a','s','h','c','r','a','f','t','S','a','v','e' };
    file.write(magic, sizeof(magic));

    int32_t gen_version = TRASHCRAFT_WORLD_GENERATOR_VERSION;
    file.write((const char*)&gen_version, sizeof(gen_version));

    char display_name_buf[64] = { 0 };
    std::strncpy(display_name_buf, g_world->info->display_name.c_str(), sizeof(display_name_buf) - 1);
    file.write(display_name_buf, sizeof(display_name_buf));

    file.write((const char*)&g_world->info->last_played, sizeof(uint64_t));

    int32_t game_mode = g_world->info->game_mode;
    file.write((const char*)&game_mode, sizeof(int32_t));

    uint8_t cheats = g_global_vars->cheats_allowed ? 1 : 0;
    file.write((const char*)&cheats, sizeof(uint8_t));

    glm::vec3 origin = g_global_vars->local_player ? g_global_vars->local_player->origin : glm::vec3(0.0f);
    file.write((const char*)&origin, sizeof(glm::vec3));

    glm::ivec2 chunk_origin = g_global_vars->local_player ? g_global_vars->local_player->chunk : glm::ivec2(0);
    file.write((const char*)&chunk_origin, sizeof(glm::ivec2));

    uint16_t rules_count = static_cast<uint16_t>(g_global_vars->game_rules.size());
    file.write((const char*)&rules_count, sizeof(uint16_t));

    for (const auto& [rule_name, rule_val] : g_global_vars->game_rules) {
        char rule_buf[64] = { 0 };
        std::strncpy(rule_buf, rule_name.c_str(), sizeof(rule_buf) - 1);
        int32_t val = rule_val;

        file.write(rule_buf, sizeof(rule_buf));
        file.write((const char*)&val, sizeof(int32_t));
    }

    item_to_id.clear();
    uint16_t items_count = static_cast<uint16_t>(g_global_vars->items.size());
    file.write((const char*)&items_count, sizeof(uint16_t));

    for (const auto& [item_name, item_ptr] : g_global_vars->items) {
        saved_item_entry_t entry{};
        entry.id = item_ptr->numeric_id;
        std::strncpy(entry.name, item_name.c_str(), sizeof(entry.name) - 1);

        file.write((const char*)&entry, sizeof(saved_item_entry_t));
    }

    file.close();

    for (auto& [hash, chunk] : g_world->chunks) {
        if (chunk) {
            save_chunk(chunk);
        }
    }
}

bool c_world_save::load_chunk(const std::string& chunk_id)
{
    std::string chunk_path = saves_path + g_world->name + "/chunks/" + chunk_id + ".tchunk";

    std::ifstream chunk_file(chunk_path, std::ios::binary | std::ios::ate);
    if (!chunk_file.is_open()) {
        return false;
    }

    if (chunk_file.tellg() != sizeof(world_chunk_t)) {
        return false;
    }

    glm::ivec2 index = glm::ivec2(0);

    chunk_file.seekg(0);
    chunk_file.read((char*)&index, sizeof(glm::ivec2));

    world_chunk_t*& chunk = g_world->chunks[CHUNK_POS_H(index.x, index.y)];
    if (!chunk) {
        chunk = new world_chunk_t();
    }
    else {
        __debugbreak();
    }

    chunk_file.seekg(0);
    chunk_file.read((char*)chunk, sizeof(world_chunk_t));

    chunk->opaque_mesh = nullptr;
    chunk->translucent_mesh = nullptr;

    chunk->last_visited = 0;
    chunk->update();

    return true;
}

bool c_world_save::save_chunk(world_chunk_t* chunk)
{
    if (!chunk) {
        return false;
    }

    std::string chunks_dir = saves_path + g_world->name + "/chunks/";

    std::string chunk_id = std::to_string(chunk->index.x) + "_" + std::to_string(chunk->index.y);
    std::string chunk_path = chunks_dir + chunk_id + ".tchunk";

    std::ofstream chunk_file(chunk_path, std::ios::binary | std::ios::trunc);
    if (chunk_file.is_open()) {
        chunk_file.write((char*)chunk, sizeof(world_chunk_t));
        chunk_file.close();

        return true;
    }

    return false;
}

void c_world_save::create_world(const std::string& display_name, const std::string& actual_name)
{
    world_info_t& info = worlds.emplace_back();
    info.display_name = display_name;
    info.actual_name = actual_name;

    std::filesystem::create_directory(saves_path + actual_name);
    std::filesystem::create_directory(saves_path + actual_name + "/chunks/");

    g_world->name = actual_name;
    g_world->info = &info;

    g_world->generate_world(128);

    save_world();
}
