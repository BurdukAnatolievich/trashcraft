#include "world.h"
#include "block.h"
#include "event_manager.h"
#include "global_vars.h"

#include <rabbit3d.h>
#include <multithreading/multithread.h>

c_world* g_world = new c_world;

c_tree::c_tree(std::string id_, c_block* log_, c_block* leaf_, int height_, int volume_)
{
    id = id_;

    log = log_;
    leaf = leaf_;

    height = height_;
    volume = volume_;
}

void c_world::initialize_structures()
{
    trees.push_back(c_tree("cherry_tree", (c_block*)g_global_vars->items["cherry_log"], (c_block*)g_global_vars->items["cherry_leaves"], 7, 3));
}

void c_world::set_generator_seed(uint32_t seed)
{
    random_seed = seed;
    tree_random_seed = seed;

    perlin_noise.reseed(seed);
}

uint32_t c_world::get_generator_seed()
{
    return random_seed;
}

void c_world::generate_tree(world_chunk_t* chunk, const glm::ivec3& chunk_position, c_tree* tree)
{
    if (!tree)
        return;

    //if (chunk_grid_position.z < 0.0f || chunk_grid_position.z > 120.0f)
    //    return;

    //if (chunk_grid_position.x < 2.0f || chunk_grid_position.x > 13.0f)
    //    return;

    //if (chunk_grid_position.y < 2.0f || chunk_grid_position.y > 13.0f)
    //    return;

    //for (int h = chunk_grid_position.z + tree->height - 1; h != chunk_grid_position.z; h--)
    //{
    //    world_chunk_slice_t* slice = &chunk->data[h];
    //    if (!slice)
    //        continue;

    //    int tree_h = h - chunk_grid_position.z;
    //    slice->data[(int)chunk_grid_position.x][(int)chunk_grid_position.y] = tree_h < (tree->height - tree->volume + 2) ? new c_block_data(tree->log) : new c_block_data(tree->leaf);

    //    int hv_diff = tree->height - tree->volume;
    //    if (tree_h > 2 && tree_h != tree->height)
    //    {
    //        for (int v = 1; v < tree->volume; v++)
    //        {
    //            if (v == 1 || tree_h <= hv_diff)
    //            {
    //                slice->data[(int)chunk_grid_position.x + v][(int)chunk_grid_position.y] = new c_block_data(tree->leaf);
    //                slice->data[(int)chunk_grid_position.x][(int)chunk_grid_position.y + v] = new c_block_data(tree->leaf);

    //                slice->data[(int)chunk_grid_position.x - v][(int)chunk_grid_position.y] = new c_block_data(tree->leaf);
    //                slice->data[(int)chunk_grid_position.x][(int)chunk_grid_position.y - v] = new c_block_data(tree->leaf);
    //            }

    //            if (v < (tree->volume - 1) && tree_h <= hv_diff)
    //            {
    //                slice->data[(int)chunk_grid_position.x - v][(int)chunk_grid_position.y - v] = new c_block_data(tree->leaf);
    //                slice->data[(int)chunk_grid_position.x - v][(int)chunk_grid_position.y + v] = new c_block_data(tree->leaf);

    //                slice->data[(int)chunk_grid_position.x + v][(int)chunk_grid_position.y + v] = new c_block_data(tree->leaf);
    //                slice->data[(int)chunk_grid_position.x + v][(int)chunk_grid_position.y - v] = new c_block_data(tree->leaf);
    //            }
    //        }
    //    }
    //}
}

void c_world::generate_chunk(world_chunk_t* chunk)
{
    int world_chunk_x = chunk->index.x * CHUNK_SIZE;
    int world_chunk_z = chunk->index.y * CHUNK_SIZE;

    auto get_block_id = [&](const std::string& name, uint16_t fallback = 0) -> uint16_t {
        auto it = g_global_vars->items.find(name);
        if (it != g_global_vars->items.end() && it->second) {
            return static_cast<c_block*>(it->second)->numeric_id;
        }
        return fallback;
        };

    uint16_t id_stone = get_block_id("stone", 1);
    uint16_t id_grass = get_block_id("grass_block", id_stone);
    uint16_t id_dirt = get_block_id("dirt", id_stone);
    uint16_t id_bedrock = get_block_id("stone", id_stone);
    uint16_t id_sand = get_block_id("sand", id_dirt);
    uint16_t id_water = get_block_id("water", 0);
    uint16_t id_log = get_block_id("cherry_log", 0);
    uint16_t id_leaves = get_block_id("cherry_leaves", 0);

    constexpr int sea_level = WORLD_HEIGHT / 3;
    int surface_map[CHUNK_SIZE][CHUNK_SIZE];

    for (int z = 0; z < CHUNK_SIZE; z++) {
        int world_z = world_chunk_z + z;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int world_x = world_chunk_x + x;

            double continental = perlin_noise.octave2D_01(world_x * 0.004, world_z * 0.004, 4);
            double hills = perlin_noise.octave2D_01(world_x * 0.015, world_z * 0.015, 4);
            double detail = perlin_noise.octave2D_01(world_x * 0.05, world_z * 0.05, 2);

            double mountain_factor = std::pow(continental, 2.2) * 38.0;
            double hill_factor = hills * 12.0;
            double detail_factor = (detail - 0.5) * 3.0;

            int height = static_cast<int>(sea_level - 4 + mountain_factor + hill_factor + detail_factor);
            height = std::clamp(height, 4, WORLD_HEIGHT - 12);

            surface_map[x][z] = height;
        }
    }

    for (int z = 0; z < CHUNK_SIZE; z++) {
        int world_z = world_chunk_z + z;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int world_x = world_chunk_x + x;
            int surface_y = surface_map[x][z];

            uint32_t col_hash = static_cast<uint32_t>(world_x * 374761393 + world_z * 668265263);
            col_hash = (col_hash ^ (col_hash >> 13)) * 1274126177;
            int dirt_depth = 3 + (col_hash & 1);
            int bedrock_height = 1 + ((col_hash >> 4) % 3);

            for (int y = 0; y < WORLD_HEIGHT; y++) {
                uint16_t block_to_set = 0;

                if (y < bedrock_height) {
                    block_to_set = id_bedrock;
                }
                else if (y <= surface_y) {
                    bool is_cave = false;
                    if (y > 4 && y < surface_y - 2) {
                        double cave_noise = perlin_noise.octave3D_01(
                            world_x * 0.045,
                            y * 0.065,
                            world_z * 0.045,
                            2
                        );

                        if (cave_noise > 0.72) {
                            is_cave = true;
                        }
                    }

                    if (!is_cave) {
                        if (y == surface_y) {
                            block_to_set = (surface_y <= sea_level + 1) ? id_sand : id_grass;
                        }
                        else if (y >= surface_y - dirt_depth) {
                            block_to_set = (surface_y <= sea_level + 1) ? id_sand : id_dirt;
                        }
                        else {
                            block_to_set = id_stone;
                        }
                    }
                }
                else if (y <= sea_level && id_water != 0) {
                    block_to_set = id_water;
                }

                chunk->blocks[REL_POS_TO_IDX(x, y, z)] = block_to_set;
            }
        }
    }

    if (id_log != 0 && id_leaves != 0) {
        for (int z = 2; z < CHUNK_SIZE - 2; z++) {
            int world_z = world_chunk_z + z;
            for (int x = 2; x < CHUNK_SIZE - 2; x++) {
                int world_x = world_chunk_x + x;
                int surface_y = surface_map[x][z];

                if (surface_y <= sea_level + 1 || surface_y > sea_level + 24) {
                    continue;
                }

                if (chunk->blocks[REL_POS_TO_IDX(x, surface_y, z)] != id_grass) {
                    continue;
                }

                uint32_t h = static_cast<uint32_t>(world_x * 198491317 + world_z * 6542989);
                h ^= (h >> 16);
                h *= 0x85ebca6b;
                h ^= (h >> 13);

                if ((h % 1000) < 15) {
                    int trunk_height = 4 + ((h >> 10) % 3);
                    if (surface_y + trunk_height + 2 >= WORLD_HEIGHT) {
                        continue;
                    }

                    for (int ly = surface_y + trunk_height - 2; ly <= surface_y + trunk_height + 1; ly++) {
                        int dy = ly - (surface_y + trunk_height);
                        int radius = (dy >= 0) ? 1 : 2;

                        for (int lz = z - radius; lz <= z + radius; lz++) {
                            for (int lx = x - radius; lx <= x + radius; lx++) {
                                if (std::abs(lx - x) == radius && std::abs(lz - z) == radius) {
                                    if (dy >= 0 || ((h >> (lx + lz)) & 1)) {
                                        continue;
                                    }
                                }

                                int idx = REL_POS_TO_IDX(lx, ly, lz);
                                if (chunk->blocks[idx] == 0) {
                                    chunk->blocks[idx] = id_leaves;
                                }
                            }
                        }
                    }

                    for (int ty = 1; ty <= trunk_height; ty++) {
                        chunk->blocks[REL_POS_TO_IDX(x, surface_y + ty, z)] = id_log;
                    }
                }
            }
        }
    }
}

void c_world::generate_world(int size)
{
    int chunks_count = size >> CHUNK_SIZE_SHIFT;
    int half_chunks_count = chunks_count >> 1;

    int gena = 0;
    for (int cz = -half_chunks_count; cz < half_chunks_count; cz++) {
        for (int cx = -half_chunks_count; cx < half_chunks_count; cx++) {
            world_chunk_t* chunk = new world_chunk_t();

            chunk->index = glm::ivec2(cx, cz);
            chunk->should_update = true;

            generate_chunk(chunk);

            chunks[CHUNK_POS_H(cx, cz)] = chunk;
            std::cout << std::hex << CHUNK_POS_H(cx, cz) << std::dec << std::endl;
            gena++;
        }
    }

    std::cout << gena << std::endl;

    update_chunks(false, true);
    build_chunks();
}

void c_world::generate_world_based_on_player_position()
{
    c_player* local = g_global_vars->local_player;
    if (!local) {
        return;
    }

    if (local->chunk == local->last_chunk && !chunks.empty()) {
        return;
    }

    int radius = g_config->render.render_distance;
    int unload_radius = radius + 2;

    //for (auto it = chunks.begin(); it != chunks.end(); ) {
    //    world_chunk_t* chunk = it->second;
    //    if (!chunk) {
    //        it = chunks.erase(it);
    //        continue;
    //    }

    //    int dx = chunk->index.x - local->chunk.x;
    //    int dz = chunk->index.y - local->chunk.y;

    //    if (std::abs(dx) > unload_radius || std::abs(dz) > unload_radius) {
    //        delete chunk;
    //        it = chunks.erase(it);
    //    }
    //    else {
    //        ++it;
    //    }
    //}

    std::vector<world_chunk_t*> chunks_to_generate;
    for (int dz = -radius; dz <= radius; dz++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dz * dz > radius * radius) {
                continue;
            }

            int cx = local->chunk.x + dx;
            int cz = local->chunk.y + dz;
            auto hash = CHUNK_POS_H(cx, cz);

            if (chunks.find(hash) != chunks.end()) {
                continue;
            }

            world_chunk_t* chunk = new world_chunk_t();
            chunk->index = glm::ivec2(cx, cz);
            chunk->should_update = false;

            chunks[hash] = chunk;
            chunks_to_generate.push_back(chunk);
        }
    }

    if (chunks_to_generate.empty()) {
        return;
    }

    constexpr size_t thread_count = 12;
    size_t actual_threads = std::min(thread_count, chunks_to_generate.size());

    std::vector<std::thread> workers;
    workers.reserve(actual_threads);

    for (size_t t = 0; t < actual_threads; ++t) {
        workers.emplace_back([this, &chunks_to_generate, t, actual_threads]() {
            for (size_t i = t; i < chunks_to_generate.size(); i += actual_threads) {
                generate_chunk(chunks_to_generate[i]);
                build_chunk_mesh(chunks_to_generate[i]);
            }
            });
    }

    for (auto& w : workers) {
        if (w.joinable()) {
            w.join();
        }
    }

    for (world_chunk_t* chunk : chunks_to_generate) {
        int cx = chunk->index.x;
        int cz = chunk->index.y;

        auto mark_neighbor = [&](int nx, int nz) {
            auto n_it = chunks.find(CHUNK_POS_H(nx, nz));
            if (n_it != chunks.end() && n_it->second) {
                n_it->second->should_update = true;
            }
            };

        mark_neighbor(cx - 1, cz);
        mark_neighbor(cx + 1, cz);
        mark_neighbor(cx, cz - 1);
        mark_neighbor(cx, cz + 1);
    }

    update_chunks(false, true);
}

bool c_world::place_block_abs(c_block* block, const glm::ivec3& abs_pos)
{
    set_block_at_abs_pos(abs_pos, block->get_numeric_id());

    glm::ivec2 local_idx = get_chunk_local_index(abs_pos);
    glm::ivec3 local_pos = get_chunk_local_pos(abs_pos);
    if (local_pos.x == 0) if (world_chunk_t* n = unrendered_chunk({ local_idx.x - 1, local_idx.y })) n->update();
    if (local_pos.x == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ local_idx.x + 1, local_idx.y })) n->update();
    if (local_pos.z == 0) if (world_chunk_t* n = unrendered_chunk({ local_idx.x, local_idx.y - 1 })) n->update();
    if (local_pos.z == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ local_idx.x, local_idx.y + 1 })) n->update();

    return true;
}

bool c_world::remove_block_abs(const glm::ivec3& abs_pos)
{
    set_block_at_abs_pos(abs_pos, 0);

    glm::ivec2 local_idx = get_chunk_local_index(abs_pos);
    glm::ivec3 local_pos = get_chunk_local_pos(abs_pos);
    if (local_pos.x == 0) if (world_chunk_t* n = unrendered_chunk({ local_idx.x - 1, local_idx.y })) n->update();
    if (local_pos.x == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ local_idx.x + 1, local_idx.y })) n->update();
    if (local_pos.z == 0) if (world_chunk_t* n = unrendered_chunk({ local_idx.x, local_idx.y - 1 })) n->update();
    if (local_pos.z == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ local_idx.x, local_idx.y + 1 })) n->update();

    return true;
}

bool c_world::place_block(c_block* block, const glm::ivec3& local_pos_rel)
{
    set_block_at_local_pos(local_pos_rel, block->get_numeric_id());

    glm::ivec2 local_idx = get_chunk_local_index(local_pos_rel);
    glm::ivec3 local_pos = get_chunk_local_pos(local_pos_rel);
    if (local_pos.x == 0) if (world_chunk_t* n = rendered_chunk({ local_idx.x - 1, local_idx.y })) n->update();
    if (local_pos.x == CHUNK_SIZE - 1) if (world_chunk_t* n = rendered_chunk({ local_idx.x + 1, local_idx.y })) n->update();
    if (local_pos.z == 0) if (world_chunk_t* n = rendered_chunk({ local_idx.x, local_idx.y - 1 })) n->update();
    if (local_pos.z == CHUNK_SIZE - 1) if (world_chunk_t* n = rendered_chunk({ local_idx.x, local_idx.y + 1 })) n->update();

    return true;
}

bool c_world::remove_block(const glm::ivec3& local_pos_rel)
{
    set_block_at_local_pos(local_pos_rel, 0);

    glm::ivec2 local_idx = get_chunk_local_index(local_pos_rel);
    glm::ivec3 local_pos = get_chunk_local_pos(local_pos_rel);
    if (local_pos.x == 0) if (world_chunk_t* n = rendered_chunk({ local_idx.x - 1, local_idx.y })) n->update();
    if (local_pos.x == CHUNK_SIZE - 1) if (world_chunk_t* n = rendered_chunk({ local_idx.x + 1, local_idx.y })) n->update();
    if (local_pos.z == 0) if (world_chunk_t* n = rendered_chunk({ local_idx.x, local_idx.y - 1 })) n->update();
    if (local_pos.z == CHUNK_SIZE - 1) if (world_chunk_t* n = rendered_chunk({ local_idx.x, local_idx.y + 1 })) n->update();

    return true;
}

bool c_world::place_block(c_block* block, world_chunk_t* chunk, const glm::ivec3& local_pos)
{
    if (!chunk) {
        return false;
    }

    chunk->set_block_id(local_pos, block->get_numeric_id());
    chunk->update();

    if (local_pos.x == 0) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x - 1, chunk->index.y })) n->update();
    if (local_pos.x == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x + 1, chunk->index.y })) n->update();
    if (local_pos.z == 0) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x, chunk->index.y - 1 })) n->update();
    if (local_pos.z == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x, chunk->index.y + 1 })) n->update();

    return true;
}

bool c_world::remove_block(world_chunk_t* chunk, const glm::ivec3& local_pos)
{
    if (!chunk) {
        return false;
    }

    chunk->set_block_id(local_pos, 0);
    chunk->update();

    if (local_pos.x == 0) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x - 1, chunk->index.y })) n->update();
    if (local_pos.x == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x + 1, chunk->index.y })) n->update();
    if (local_pos.z == 0) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x, chunk->index.y - 1 })) n->update();
    if (local_pos.z == CHUNK_SIZE - 1) if (world_chunk_t* n = unrendered_chunk({ chunk->index.x, chunk->index.y + 1 })) n->update();

    return true;
}

void c_world::build_block_mesh(c_mesh* mesh, glm::vec3 local_pos, c_block* block, int side)
{
    int material_side = std::max(side - e_side::S_LEFT, 0);
    c_texture* material = block->materials[material_side] ? block->materials[material_side] : block->materials[c_block::e_material::SIDE];

    glm::vec3 min = local_pos + block->min, max = local_pos + block->max;
    glm::vec2 uv_min = material->atlas_uv_min, uv_max = material->atlas_uv_max;

    std::array<c_mesh::vertex_t, 6> v;

    switch (side) {
    case e_side::S_FRONT: {
        glm::vec3 n{ 0.0f, 0.0f, -1.0f };
        v = {
            c_mesh::vertex_t({ { max.x, max.y, min.z }, n, { uv_min.x, uv_min.y } }),
            c_mesh::vertex_t({ { max.x, min.y, min.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { min.x, max.y, min.z }, n, { uv_max.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, max.y, min.z }, n, { uv_max.x, uv_min.y } }),
            c_mesh::vertex_t({ { max.x, min.y, min.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { min.x, min.y, min.z }, n, { uv_max.x, uv_max.y } })
        };
        break;
    }
    case e_side::S_BACK: {
        glm::vec3 n{ 0.0f, 0.0f, 1.0f };
        v = {
            c_mesh::vertex_t({ { max.x, min.y, max.z }, n, { uv_max.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, max.y, max.z }, n, { uv_max.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, min.y, max.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { min.x, max.y, max.z }, n, { uv_min.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, min.y, max.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, max.y, max.z }, n, { uv_max.x, uv_min.y } })
        };
        break;
    }
    case e_side::S_RIGHT: {
        glm::vec3 n{ 1.0f, 0.0f, 0.0f };
        v = {
            c_mesh::vertex_t({ { max.x, min.y, min.z }, n, { uv_max.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, max.y, max.z }, n, { uv_min.x, uv_min.y } }),
            c_mesh::vertex_t({ { max.x, min.y, max.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, min.y, min.z }, n, { uv_max.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, max.y, min.z }, n, { uv_max.x, uv_min.y } }),
            c_mesh::vertex_t({ { max.x, max.y, max.z }, n, { uv_min.x, uv_min.y } })
        };
        break;
    }
    case e_side::S_LEFT: {
        glm::vec3 n{ -1.0f, 0.0f, 0.0f };
        v = {
            c_mesh::vertex_t({ { min.x, min.y, max.z }, n, { uv_max.x, uv_max.y } }),
            c_mesh::vertex_t({ { min.x, max.y, min.z }, n, { uv_min.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, min.y, min.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { min.x, min.y, max.z }, n, { uv_max.x, uv_max.y } }),
            c_mesh::vertex_t({ { min.x, max.y, max.z }, n, { uv_max.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, max.y, min.z }, n, { uv_min.x, uv_min.y } })
        };
        break;
    }
    case e_side::S_TOP: {
        glm::vec3 n{ 0.0f, 1.0f, 0.0f };
        v = {
            c_mesh::vertex_t({ { min.x, max.y, max.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, max.y, min.z }, n, { uv_max.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, max.y, min.z }, n, { uv_min.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, max.y, max.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, max.y, max.z }, n, { uv_max.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, max.y, min.z }, n, { uv_max.x, uv_min.y } })
        };
        break;
    }
    case e_side::S_BOTTOM: {
        glm::vec3 n{ 0.0f, -1.0f, 0.0f };
        v = {
            c_mesh::vertex_t({ { min.x, min.y, min.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, min.y, max.z }, n, { uv_max.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, min.y, max.z }, n, { uv_min.x, uv_min.y } }),
            c_mesh::vertex_t({ { min.x, min.y, min.z }, n, { uv_min.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, min.y, min.z }, n, { uv_max.x, uv_max.y } }),
            c_mesh::vertex_t({ { max.x, min.y, max.z }, n, { uv_max.x, uv_min.y } })
        };
        break;
    }
    default:
        return;
    }

    mesh->vertices.insert(mesh->vertices.end(), v.begin(), v.end());
}

void c_world::build_chunk_mesh(world_chunk_t* chunk)
{
    if (!chunk)
        return;

    size_t last_opaque_mesh_size = 0, last_translucent_mesh_size = 0;

    c_mesh* opaque_mesh = chunk->opaque_mesh;
    if (opaque_mesh) {
        last_opaque_mesh_size = opaque_mesh->vb_size;

        opaque_mesh->should_reassemble_vb = true;
        opaque_mesh->clear();
    }
    else {
        opaque_mesh = chunk->opaque_mesh = new c_mesh("o_chunk##" + std::to_string(chunk->index.x) + std::to_string(chunk->index.y), glm::vec3(glm::vec2(chunk->index) * glm::vec2(CHUNK_SIZE, CHUNK_SIZE), 0.0f), &g_texture_manager->texture_atlas);
    }

    if (last_opaque_mesh_size) {
        opaque_mesh->vertices.reserve(last_opaque_mesh_size);
    }

    c_mesh* translucent_mesh = chunk->translucent_mesh;
    if (translucent_mesh) {
        last_translucent_mesh_size = translucent_mesh->vb_size;

        translucent_mesh->should_reassemble_vb = true;
        translucent_mesh->clear();
    }
    else {
        translucent_mesh = chunk->translucent_mesh = new c_mesh("t_chunk##" + std::to_string(chunk->index.x) + std::to_string(chunk->index.y), glm::vec3(glm::vec2(chunk->index) * glm::vec2(CHUNK_SIZE, CHUNK_SIZE), 0.0f), &g_texture_manager->texture_atlas);
    }

    if (last_translucent_mesh_size) {
        translucent_mesh->vertices.reserve(last_translucent_mesh_size);
    }

    world_chunk_t* neighbors[6] = {
        unrendered_chunk({ chunk->index.x, chunk->index.y - 1 }),
        unrendered_chunk({ chunk->index.x, chunk->index.y + 1 }),
        unrendered_chunk({ chunk->index.x + 1, chunk->index.y }),
        unrendered_chunk({ chunk->index.x - 1, chunk->index.y }),
        nullptr, 
        nullptr 
    };

    glm::ivec3 offsets[6] = {
        {0, 0, -1},
        {0, 0, 1},
        {1, 0, 0}, 
        {-1, 0, 0},
        {0, 1, 0}, 
        {0, -1, 0}
    };
    
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                uint16_t block_id = chunk->get_block_id({ x, y, z });
                if (!block_id)
                    continue;

                c_block* block = (c_block*)g_global_vars->mapped_items[block_id];
                if (!block || block->type != c_item::e_type::BLOCK) {
                    continue;
                }

                bool is_water = block->is_transparent && !block->is_solid;

                glm::vec3 position = glm::vec3(x, y, z);
                for (int side = 0; side < S_MAX; side++) {
                    glm::ivec3 neighbor_pos = glm::ivec3(x, y, z) + offsets[side];

                    bool is_occluded = false; uint16_t occlusion_id = 0;
                    if (neighbor_pos.y < 0 || neighbor_pos.y >= WORLD_HEIGHT) {
                        is_occluded = false;
                    }
                    else if (neighbor_pos.x >= 0 && neighbor_pos.x < CHUNK_SIZE &&
                        neighbor_pos.z >= 0 && neighbor_pos.z < CHUNK_SIZE) {
                        occlusion_id = chunk->get_block_id(neighbor_pos);
                    }
                    else {
                        world_chunk_t* neighbor_chunk = neighbors[side];
                        if (neighbor_chunk) {
                            occlusion_id = neighbor_chunk->get_block_id({
                                neighbor_pos.x & (CHUNK_SIZE - 1),
                                neighbor_pos.y,
                                neighbor_pos.z & (CHUNK_SIZE - 1)
                                });
                        }
                    }

                    if (occlusion_id != 0) {
                        bool neighbor_opaque = !g_global_vars->mapped_transparency[occlusion_id];
                        is_occluded = neighbor_opaque || (is_water && occlusion_id == block_id);
                    }

                    if (is_occluded)
                        continue;

                    build_block_mesh(block->is_transparent && !block->is_solid ? translucent_mesh : opaque_mesh, position, block, side);
                }
            }
        }
    }

    opaque_mesh->should_reassemble_vb = true;
    translucent_mesh->should_reassemble_vb = true;
}

void c_world::build_chunks()
{
    if (!g_global_vars->scene || !g_global_vars->camera)
        return;

    int render_distance = g_config->render.render_distance;
    for (int cz = -render_distance; cz <= render_distance; cz++) {
        for (int cx = -render_distance; cx <= render_distance; cx++) {
            world_chunk_t* chunk = rendered_chunk({ cx, cz });
            if (!chunk) {
                continue;
            }

            build_chunk_mesh(chunk);
        }
    }
}

void c_world::update_chunks(bool update_tick, bool update_render)
{
    c_player* local = g_global_vars->local_player;
    if (!local)
        return;

    if (update_tick) {

    }

    if (update_render) {
        world_chunk_t* b_render_chunks[(MAX_RENDER_DIST + 1) * (MAX_RENDER_DIST + 1)];

        int render_distance = g_config->render.render_distance;
        for (int cz = -render_distance; cz <= render_distance; cz++) {
            for (int cx = -render_distance; cx <= render_distance; cx++) {
                int abs_cx = local->chunk.x + cx;
                int abs_cz = local->chunk.y + cz;

                uint32_t x = (uint32_t)(cx + MAX_RENDER_DIST_R);
                uint32_t z = (uint32_t)(cz + MAX_RENDER_DIST_R);

                world_chunk_t* chunk = unrendered_chunk({ abs_cx, abs_cz });
                if (chunk && chunk->should_update) {
                    build_chunk_mesh(chunk);
                    chunk->should_update = false;
                }

                b_render_chunks[x + (z * (uint32_t)(MAX_RENDER_DIST + 1))] = chunk;
            }
        }

        //std::lock_guard<std::mutex> chunk_filler(chunk_filler_mutex);
        std::memcpy(render_chunks, b_render_chunks, sizeof(render_chunks));
    }
}

void c_world::clear_chunk(world_chunk_t* chunk)
{
    c_mesh* opaque_mesh = chunk->opaque_mesh;
    if (opaque_mesh) {
        opaque_mesh->free();
        opaque_mesh->clear();
        delete opaque_mesh;
    }

    c_mesh* translucent_mesh = chunk->translucent_mesh;
    if (translucent_mesh) {
        translucent_mesh->free();
        translucent_mesh->clear();
        delete translucent_mesh;
    }
    delete chunk;
}

void c_world::clear_world()
{
    std::memset(render_chunks, 0, sizeof(render_chunks));

    for (auto& _chunk : chunks) {
        world_chunk_t* chunk = _chunk.second;
        if (!chunk)
            continue;

        clear_chunk(chunk);
    }

    chunks.clear();
}
