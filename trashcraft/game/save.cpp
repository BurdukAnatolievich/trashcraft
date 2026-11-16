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
        std::ifstream world_info(world_info_path.string());
        if (world_info.is_open())
        {
            std::vector<std::string> buffer;
            while (std::getline(world_info, buffer.emplace_back())) {}

            if (buffer.size() >= 2 && buffer.at(0) == "TrashcraftSave" && buffer.at(1)[0] == TRASHCRAFT_WORLD_GENERATOR_VERSION)
            {
                int id = worlds.size();
                worlds.push_back({ buffer.at(2), entry.path().filename().string(), entry.path().string() + "/chunks/", { }, buffer });

                world_info_t& info = worlds.at(id);
                for (const auto& entry : std::filesystem::directory_iterator(info.chunks_path))
                {
                    if (entry.path().extension().string() != ".tchunk")
                        continue;

                    info.chunks.push_back(entry.path().stem().string());
                }
            }

            world_info.close();
        }
    }
}

void c_world_save::load_world(int id)
{
    if (id < 0 || id >= worlds.size())
        return;

    world_info_t& info = worlds.at(id);
    if (info.parsed_info.empty() || info.parsed_info.size() < 7)
        return;

    g_world->name = info.actual_name;
    g_world->info = &info;

    float x = std::stof(info.parsed_info.at(3));
    float y = std::stof(info.parsed_info.at(4));
    float z = std::stof(info.parsed_info.at(5));

    g_global_vars->local_player->set_origin(glm::vec3(x, y, z));

    for (int i = 6; i < info.parsed_info.size(); i++)
    {
        std::vector<std::string> splitted_string;

        std::stringstream stream;
        stream << info.parsed_info.at(i);

        std::string temp;
        while (std::getline(stream, temp, ':')) { splitted_string.push_back(temp); }

        if (splitted_string.size() != 2)
            continue;

        short id = std::stoi(splitted_string[0]);
        indices_to_item[id] = g_global_vars->items[splitted_string[1]];
    }

    for (int i = 0; i < info.chunks.size(); i++)
    {
        load_chunk(info.chunks.at(i));
    }

    g_world->build_chunks();

    indices_to_item.clear();
}

void c_world_save::save_world()
{
    std::ofstream world_info(saves_path + g_world->name + "/world_info.tcworld");
    if (world_info.is_open())
    {
        world_info << "TrashcraftSave\n";
        world_info << TRASHCRAFT_WORLD_GENERATOR_VERSION << "\n";
        world_info << g_world->info->display_name << "\n";

        if (g_global_vars->local_player)
        {
            glm::vec3 origin = g_global_vars->local_player->get_origin();
            world_info << origin.x << "\n" << origin.y << "\n" << origin.z << "\n";
        }

        int item_id = 0;
        for (auto& pair : g_global_vars->items)
        {
            world_info << std::dec << item_id << ":" << pair.first << "\n";
            item_to_indices[pair.second] = item_id;
            item_id++;
        }

        world_info.close();
    }

    //for (int i = 0; i < g_world->chunks.size(); i++)
    //{
    //    world_chunk_t* chunk = &g_world->chunks.at(i);

    //    save_chunk(chunk);
    //}

    item_to_indices.clear();
}

void c_world_save::load_chunk(world_chunk_t* chunk)
{

}

void c_world_save::load_chunk(std::string chunk_id)
{
    std::string chunk_dir_path = saves_path + g_world->name + "/chunks/" + chunk_id + ".tchunk";
    std::ifstream chunk_file(chunk_dir_path, std::ios::binary);
    if (!chunk_file.is_open())
        return;

    save_chunk_t* save_chunk = new save_chunk_t;

    chunk_file.read((char*)save_chunk, sizeof(save_chunk_t));
    chunk_file.close();

    //world_chunk_t* chunk = &g_world->chunks.emplace_back();
    //chunk->position = save_chunk->position;
    //chunk->index = save_chunk->index;

    //for (int x = 0; x < 16; x++)
    //{
    //    for (int y = 0; y < 16; y++)
    //    {
    //        for (int h = 0; h < 128; h++)
    //        {
    //            short block_id = save_chunk->data[h].data[x][y];
    //            if (block_id == -1)
    //                continue;

    //            c_block* block = (c_block*)indices_to_item[block_id];
    //            if (!block || block->type != c_item::e_type::BLOCK)
    //                continue;

    //            //c_block_data* new_data = nullptr;
    //            //switch (block->block_type)
    //            //{
    //            //case c_block::e_block_type::DEFAULT:
    //            //    new_data = new c_block_data(block);
    //            //    break;
    //            //case c_block::e_block_type::CHEST:
    //            //    new_data = new c_chest(block);
    //            //    break;
    //            //case c_block::e_block_type::DOOR:
    //            //    new_data = new c_door(block);
    //            //    break;
    //            //case c_block::e_block_type::JUKEBOX:
    //            //    new_data = new c_jukebox(block);
    //            //    break;
    //            //}

    //            //chunk->data[h].data[x][y] = new_data;
    //        }
    //    }
    //}

    //chunk->render_data.mesh = new c_mesh("chunk##" + std::to_string(int(chunk->position.x)) + std::to_string(int(chunk->position.y)), chunk->position, &g_texture_manager->texture_atlas);
    //g_world->create_chunk_scene_objects(chunk);

    delete save_chunk;
}

void c_world_save::save_chunk(world_chunk_t* chunk)
{
    //save_chunk_t* save_chunk = new save_chunk_t;
    //std::memset(save_chunk, 0, sizeof(save_chunk_t));

    //save_chunk->index = chunk->index;

    //for (int h = 0; h < WORLD_HEIGHT; h++) {
    //    for (int x = 0; x < CHUNK_SIZE; x++) {
    //        for (int y = 0; y < CHUNK_SIZE; y++) {
    //            //c_block_data* block = chunk->data[h].data[x][y];
    //            //if (!block || !block->block)
    //            //{
    //            //    save_chunk->data[h].data[x][y] = -1;
    //            //    continue;
    //            //}

    //            //save_chunk->data[h].data[x][y] = item_to_indices[block->block];
    //        }
    //    }
    //}

    //std::string chunk_dir_path = saves_path + g_world->name + "/chunks/" + chunk->render_data.mesh->name + ".tchunk";
    //std::ofstream chunk_file(chunk_dir_path, std::ios::binary);
    //if (chunk_file.is_open())
    //{
    //    chunk_file.write((char*)save_chunk, sizeof(save_chunk_t));
    //    chunk_file.close();
    //}

    //delete save_chunk;
}

void c_world_save::create_world(const std::string& display_name, const std::string& actual_name)
{
    world_info_t& info = worlds.emplace_back();
    info.display_name = display_name;
    info.actual_name = actual_name;
    info.chunks_path = saves_path + actual_name + "/chunks/";

    std::filesystem::create_directory(saves_path + actual_name);
    std::filesystem::create_directory(info.chunks_path);

    g_world->name = actual_name;
    g_world->info = &info;

    g_world->generate_world(128);

    //save_world();

    //g_world->create_scene_objects();
}
