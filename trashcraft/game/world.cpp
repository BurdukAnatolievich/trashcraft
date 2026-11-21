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
    //trees.push_back(c_tree("cherry_tree", (c_block*)g_global_vars->items["cherry_log"], (c_block*)g_global_vars->items["cherry_leaves"], 7, 3));
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

    const uint16_t id_air = 0;
    const uint16_t id_stone = get_block_id("stone", 1);
    const uint16_t id_grass = get_block_id("grass_block", id_stone);
    const uint16_t id_dirt = get_block_id("dirt", id_stone);
    const uint16_t id_bedrock = get_block_id("bedrock", id_stone);
    const uint16_t id_sand = get_block_id("sand", id_dirt);
    const uint16_t id_gravel = get_block_id("gravel", id_stone);
    const uint16_t id_water = get_block_id("water", 0);
    const uint16_t id_lava = get_block_id("lava", id_air);
    const uint16_t id_log = get_block_id("oak_log", 0);
    uint16_t id_leaves = get_block_id("oak_leaves", 0);
    if (id_leaves == 0) id_leaves = get_block_id("leaves", 0);

    const uint16_t id_coal = get_block_id("coal_ore", id_stone);
    const uint16_t id_iron = get_block_id("iron_ore", id_stone);
    const uint16_t id_gold = get_block_id("gold_ore", id_stone);
    const uint16_t id_diamond = get_block_id("diamond_ore", id_stone);

    constexpr int MAX_HEIGHT = 128;
    constexpr int SEA_LEVEL = 62;

    auto smoothstep = [](double edge0, double edge1, double x) -> double {
        double t = std::clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
        return t * t * (3.0 - 2.0 * t);
        };

    for (int x = 0; x < CHUNK_SIZE; x++) {
        int wx = world_chunk_x + x;

        for (int z = 0; z < CHUNK_SIZE; z++) {
            int wz = world_chunk_z + z;

            double cont = perlin_noise.octave2D_01(wx * 0.0014, wz * 0.0014, 4);
            double base_height = 0.0;
            if (cont < 0.36) {
                base_height = 30.0 + (cont / 0.36) * 24.0;
            }
            else if (cont < 0.50) {
                double t = smoothstep(0.36, 0.50, cont);
                base_height = 54.0 + t * 11.0;
            }
            else {
                double t = (cont - 0.50) / 0.50;
                base_height = 65.0 + t * 8.0;
            }

            double land_mask = smoothstep(0.42, 0.55, cont);
            double hills = (perlin_noise.octave2D_01(wx * 0.011, wz * 0.011, 3) - 0.5) * 10.0;
            base_height += hills * land_mask;

            double inland_mask = smoothstep(0.50, 0.65, cont);
            double m_noise = perlin_noise.octave2D_01(wx * 0.0045 + 1500.0, wz * 0.0045 + 1500.0, 3);
            double mountain_strength = smoothstep(0.45, 0.80, m_noise) * inland_mask;

            double ridge_raw = perlin_noise.octave2D_01(wx * 0.012 + 400.0, wz * 0.012 + 400.0, 4);
            double ridge = 1.0 - std::abs(ridge_raw * 2.0 - 1.0);
            base_height += mountain_strength * (18.0 + std::pow(ridge, 1.5) * 32.0);

            double cliff_power = mountain_strength * 22.0;

            int stone_depth = 0;

            uint32_t col_seed = static_cast<uint32_t>(wx * 73856093 ^ wz * 19349663);
            std::mt19937 rng(col_seed ^ random_seed);

            for (int y = MAX_HEIGHT - 1; y >= 0; y--) {
                glm::ivec3 pos(x, y, z);

                if (y == 0 || (y <= 4 && (rng() % (y + 1) == 0))) {
                    chunk->set_block(pos, id_bedrock);
                    continue;
                }

                double noise_3d = 0.0;
                if (cliff_power > 0.2 && y > SEA_LEVEL) {
                    noise_3d = perlin_noise.octave3D(wx * 0.028, y * 0.055, wz * 0.028, 3) * cliff_power;
                }

                double density = (base_height - y) + noise_3d;
                if (y > 112) {
                    density -= (y - 112) * 2.5;
                }

                bool is_cave = false;
                if (y > 4 && y < MAX_HEIGHT - 12 && density > 0.0) {
                    // Переводим octave3D из [-1..1] в понятный диапазон [0..1]
                    double n1 = perlin_noise.octave3D(wx * 0.014 + 3000.0, y * 0.025, wz * 0.014 + 3000.0, 2) * 0.5 + 0.5;
                    double n2 = perlin_noise.octave3D(wx * 0.018 + 7000.0, y * 0.030, wz * 0.018 + 7000.0, 2) * 0.5 + 0.5;

                    bool big_cheese = (n1 > 0.88);
                    bool wide_tunnel = (std::abs(n1 - 0.5) < 0.075 && std::abs(n2 - 0.5) < 0.075);

                    if (big_cheese || wide_tunnel) {
                        if (density > 7.0 || (cliff_power > 5.0 && density > 2.5)) {
                            is_cave = true;
                        }
                    }
                }

                if (density > 0.0 && !is_cave) {
                    uint16_t block_to_place = id_stone;

                    bool is_steep_cliff = (cliff_power > 6.0 && std::abs(noise_3d) > 6.5 && y > SEA_LEVEL + 4);
                    if (stone_depth == 0) {
                        if (y < SEA_LEVEL - 1) {
                            block_to_place = (perlin_noise.octave2D_01(wx * 0.04, wz * 0.04, 2) > 0.6) ? id_gravel : id_sand;
                        }
                        else if (y <= SEA_LEVEL + 2) {
                            block_to_place = id_sand;
                        }
                        else if (is_steep_cliff || y > 98) {
                            block_to_place = id_stone;
                        }
                        else {
                            block_to_place = id_grass;
                        }
                    }
                    else if (stone_depth <= 3) {
                        if (y <= SEA_LEVEL + 2) {
                            block_to_place = id_sand;
                        }
                        else if (is_steep_cliff || y > 98) {
                            block_to_place = id_stone;
                        }
                        else {
                            block_to_place = id_dirt;
                        }
                    }

                    chunk->set_block(pos, block_to_place);
                    stone_depth++;
                }
                else {
                    stone_depth = 0;

                    if (is_cave && y <= 10) {
                        chunk->set_block(pos, id_lava);
                    }
                    else if (y < SEA_LEVEL && !is_cave) {
                        chunk->set_block(pos, id_water);
                    }
                    else {
                        chunk->set_block(pos, id_air);
                    }
                }
            }
        }
    }

    uint32_t chunk_seed = static_cast<uint32_t>(chunk->index.x * 341873128712ULL + chunk->index.y * 132897987541ULL);
    std::mt19937 chunk_rng(chunk_seed ^ random_seed);

    auto spawn_ore_vein = [&](uint16_t ore_id, int count, int max_y, int vein_size) {
        if (ore_id == id_stone) return;
        for (int i = 0; i < count; i++) {
            int vx = chunk_rng() % CHUNK_SIZE;
            int vy = 5 + (chunk_rng() % std::max(1, max_y - 5));
            int vz = chunk_rng() % CHUNK_SIZE;

            for (int b = 0; b < vein_size; b++) {
                glm::ivec3 p(vx, vy, vz);
                if (vx >= 0 && vx < CHUNK_SIZE && vy > 0 && vy < MAX_HEIGHT && vz >= 0 && vz < CHUNK_SIZE) {
                    if (chunk->get_block(p) == id_stone) {
                        chunk->set_block(p, ore_id);
                    }
                }
                vx += (int)(chunk_rng() % 3) - 1;
                vy += (int)(chunk_rng() % 3) - 1;
                vz += (int)(chunk_rng() % 3) - 1;
            }
        }
        };

    spawn_ore_vein(id_coal, 16, 110, 10);
    spawn_ore_vein(id_iron, 14, 64, 7);
    spawn_ore_vein(id_gold, 4, 32, 6);
    spawn_ore_vein(id_diamond, 2, 16, 5);

    if (id_log != 0 && id_leaves != 0) {
        double forest_density = perlin_noise.octave2D_01(world_chunk_x * 0.008 + 900.0, world_chunk_z * 0.008 + 900.0, 2);
        int trees_in_chunk = (forest_density > 0.55) ? (3 + chunk_rng() % 4) : (chunk_rng() % 2);

        for (int t = 0; t < trees_in_chunk; t++) {
            int tx = 2 + (chunk_rng() % (CHUNK_SIZE - 4));
            int tz = 2 + (chunk_rng() % (CHUNK_SIZE - 4));

            for (int y = MAX_HEIGHT - 12; y > SEA_LEVEL + 2; y--) {
                if (chunk->get_block(glm::ivec3(tx, y, tz)) == id_grass) {
                    int trunk_h = 4 + (chunk_rng() % 3);

                    for (int ly = y + trunk_h - 2; ly <= y + trunk_h + 1; ly++) {
                        int radius = (ly >= y + trunk_h) ? 1 : 2;
                        for (int lx = tx - radius; lx <= tx + radius; lx++) {
                                for (int lz = tz - radius; lz <= tz + radius; lz++) {
                                    if (std::abs(lx - tx) == radius && std::abs(lz - tz) == radius && (chunk_rng() % 2 == 0)) continue;
                                    glm::ivec3 lpos(lx, ly, lz);
                                    if (chunk->get_block(lpos) == id_air) {
                                        chunk->set_block(lpos, id_leaves);
                                    }
                                }
                        }
                    }
                    for (int ty = 1; ty <= trunk_h; ty++) {
                        chunk->set_block(glm::ivec3(tx, y + ty, tz), id_log);
                    }
                    break;
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

    for (auto it = chunks.begin(); it != chunks.end(); ) {
        world_chunk_t* chunk = it->second;
        if (!chunk) {
            it = chunks.erase(it);
            continue;
        }

        int dx = chunk->index.x - local->chunk.x;
        int dz = chunk->index.y - local->chunk.y;

        if (std::abs(dx) > unload_radius || std::abs(dz) > unload_radius) {
            delete chunk;
            it = chunks.erase(it);
        }
        else {
            ++it;
        }
    }

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
        translucent_mesh = chunk->translucent_mesh = new c_mesh("t_chunk##" + std::to_string(chunk->index.x) + std::to_string(chunk->index.y), glm::vec3(glm::vec2(chunk->index) * glm::vec2(CHUNK_SIZE, CHUNK_SIZE), 0.0f), &g_texture_manager->animated_atlas);
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
                    chunk->should_save = true;
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
