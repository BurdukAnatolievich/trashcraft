#pragma once
#include <vector>
#include <deque>
#include <mutex>

#include "save.h"

#include <perlin_noise/PerlinNoise.hpp>

#define REL_POS_TO_IDX(x, y, z) (uint32_t)((uint32_t)(x) + ((uint32_t)(z)*CHUNK_SIZE) + ((uint32_t)(y)*CHUNK_SIZE*CHUNK_SIZE))

#define GET_BLOCK_IDX(block) block & 0x3FFF
#define SET_BLOCK_IDX(block, index) block &= ~0x3FFF; block |= index & 0x3FFF;

#define CHUNK_POS_R(x, z) x + (z * (MAX_RENDER_DIST+1))
#define CHUNK_POS_H(x, z) (uint64_t)((((uint64_t)(uint32_t)x) << 32) | (uint64_t)(uint32_t)z)

class c_mesh;
class c_block;
class c_block_data;
class c_scene_object;
struct world_info_t;

enum e_face_direction
{
    NORTH,
    WEST,
    EAST,
    SOUTH,
};

struct world_chunk_t
{
    enum e_priority : uint8_t
    {
        P_FIRST,
        P_SECOND,
        P_LAST
    };

    c_mesh* opaque_mesh = nullptr, *translucent_mesh = nullptr;
    uint32_t blocks[CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT] = { 0 };
    uint8_t block_light[CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT] = { 0 };

    glm::ivec2 index = glm::ivec2(0, 0);
    uint32_t last_visited = 0;

    bool should_update = false, is_ghost = false;
    uint8_t update_priority = 0;

    inline uint16_t get_block_id(const glm::ivec3& pos) {
        uint32_t& block = blocks[REL_POS_TO_IDX(pos.x, pos.y, pos.z)];
        return block & 0x3FFF;
    }

    inline void set_block_id(const glm::ivec3& pos, uint16_t id) {
        uint32_t& block = blocks[REL_POS_TO_IDX(pos.x, pos.y, pos.z)];
        block = (block & ~0x3FFF) | (id & 0x3FFF);
    }

    inline uint32_t get_block(const glm::ivec3& pos) {
        return blocks[REL_POS_TO_IDX(pos.x, pos.y, pos.z)];
    }

    inline void set_block(const glm::ivec3& pos, uint32_t block) {
        blocks[REL_POS_TO_IDX(pos.x, pos.y, pos.z)] = block;
    }

    inline uint8_t get_block_light(const glm::ivec3& pos) {
        return block_light[REL_POS_TO_IDX(pos.x, pos.y, pos.z)];
    }

    inline void set_block_light(const glm::ivec3& pos, uint8_t light) {
        block_light[REL_POS_TO_IDX(pos.x, pos.y, pos.z)] = light;
    }

    inline glm::ivec3 to_abs_position(const glm::ivec3& pos) {
        glm::ivec2 abs_chunk_pos = index * glm::ivec2(CHUNK_SIZE, CHUNK_SIZE);
        return glm::ivec3(abs_chunk_pos.x + pos.x, pos.y, abs_chunk_pos.y + pos.z);
    }

    inline void update(uint8_t priority = P_FIRST) {
        should_update = true;
        update_priority = priority;
    }
};

class c_structure
{
public:
    enum e_type
    {
        TREE,
    };

    int type = 0;
    std::string id = "";
};

class c_tree : public c_structure
{
public:
    c_tree(std::string id, c_block* log, c_block* leaf, int height = 5, int volume = 1);

    c_block* log = nullptr;
    c_block* leaf = nullptr;

    int height = 5;
    int volume = 1;
};

class c_world
{
    std::vector<c_tree> trees;

    siv::PerlinNoise perlin_noise;

    uint32_t random_seed = 0;
    uint32_t tree_random_seed = 0;
public:
    std::string name = "";
    c_world_save::world_info_t* info = nullptr;

    std::unordered_map<uint64_t, world_chunk_t*> chunks;
    world_chunk_t* render_chunks[(MAX_RENDER_DIST + 1) * (MAX_RENDER_DIST + 1)]; // for 16x16 render distance, player is in the center

    std::mutex chunk_filler_mutex;

    enum e_side
    {
        S_FRONT,
        S_BACK,
        S_RIGHT,
        S_LEFT,
        S_TOP,
        S_BOTTOM,
        S_MAX
    };

    void initialize_structures();

    void set_generator_seed(uint32_t seed);
    uint32_t get_generator_seed();

    void generate_tree(world_chunk_t* chunk, const glm::ivec3& chunk_pos, c_tree* tree);
    void generate_chunk(world_chunk_t* chunk);

    void generate_world(int size);
    void generate_world_based_on_player_position();

    bool place_block_abs(c_block* block, const glm::ivec3& abs_pos);
    bool remove_block_abs(const glm::ivec3& abs_pos);

    bool place_block(c_block* block, const glm::ivec3& local_pos);
    bool remove_block(const glm::ivec3& local_pos);

    bool place_block(c_block* block, world_chunk_t* chunk, const glm::ivec3& local_pos);
    bool remove_block(world_chunk_t* chunk, const glm::ivec3& local_pos);

    void build_block_mesh(c_mesh* mesh, glm::vec3 local_pos, c_block* block, int side);
    void build_chunk_mesh(world_chunk_t* chunk);
    void build_chunks();

    void update_chunks(bool update_tick = true, bool update_render = false);

    //void render_opaque(int render_distance);
    //void render_translucent(int render_distance);

    void clear_chunk(world_chunk_t* chunk);
    void clear_world();

    inline glm::ivec2 get_chunk_local_index(const glm::ivec3& local_position) {
        return { local_position.x >> CHUNK_SIZE_SHIFT, local_position.z >> CHUNK_SIZE_SHIFT };
    }

    inline glm::ivec3 get_chunk_local_pos(const glm::ivec3& local_position) {
        return { local_position.x & (CHUNK_SIZE - 1), local_position.y, local_position.z & (CHUNK_SIZE - 1) };
    }

    inline world_chunk_t* get_chunk_at_local_pos(const glm::ivec3& local_position) {
        return rendered_chunk({ local_position.x >> CHUNK_SIZE_SHIFT, local_position.z >> CHUNK_SIZE_SHIFT });
    }

    inline uint32_t get_block_at_local_pos(const glm::ivec3& local_position) {
        if (local_position.y < 0) {
            return 0;
        }

        world_chunk_t* chunk = get_chunk_at_local_pos(local_position);
        if (!chunk) {
            return 0x13371337;
        }

        return chunk->get_block({ local_position.x & (CHUNK_SIZE - 1), local_position.y, local_position.z & (CHUNK_SIZE - 1) });
    }

    inline void set_block_at_local_pos(const glm::ivec3& local_position, uint32_t block) {
        if (local_position.y < 0) {
            return;
        }

        world_chunk_t* chunk = get_chunk_at_local_pos(local_position);
        if (!chunk) {
            return;
        }

        chunk->set_block({ local_position.x & (CHUNK_SIZE - 1), local_position.y, local_position.z & (CHUNK_SIZE - 1) }, block);
        chunk->update();
    }

    inline world_chunk_t* get_chunk_at_abs_pos(const glm::ivec3& abs_position) {
        return unrendered_chunk({ abs_position.x >> CHUNK_SIZE_SHIFT, abs_position.z >> CHUNK_SIZE_SHIFT });
    }

    inline uint32_t get_block_at_abs_pos(const glm::ivec3& abs_position) {
        if (abs_position.y < 0) {
            return 0;
        }

        world_chunk_t* chunk = get_chunk_at_abs_pos(abs_position);
        if (!chunk) {
            return 0x13371337;
        }

        return chunk->get_block({ abs_position.x & (CHUNK_SIZE - 1), abs_position.y, abs_position.z & (CHUNK_SIZE - 1) });
    }

    inline void set_block_at_abs_pos(const glm::ivec3& abs_position, uint32_t block) {
        if (abs_position.y < 0) {
            return;
        }

        world_chunk_t* chunk = get_chunk_at_abs_pos(abs_position);
        if (!chunk) {
            return;
        }

        chunk->set_block({ abs_position.x & (CHUNK_SIZE - 1), abs_position.y, abs_position.z & (CHUNK_SIZE - 1) }, block);
        chunk->update();
    }

    inline glm::ivec3 get_abs_position(world_chunk_t* chunk, const glm::ivec3& chunk_position) {
        return chunk->to_abs_position(chunk_position);
    }

    inline world_chunk_t*& rendered_chunk(glm::ivec2 c) {
        uint32_t x = (uint32_t)(int32_t)((int32_t)c.x + (int32_t)(MAX_RENDER_DIST_R));
        uint32_t z = (uint32_t)(int32_t)((int32_t)c.y + (int32_t)(MAX_RENDER_DIST_R));

        return render_chunks[x + (z * (uint32_t)(MAX_RENDER_DIST + 1))];
    }

    inline world_chunk_t* unrendered_chunk(glm::ivec2 c) {
        auto iterator = chunks.find(CHUNK_POS_H(c.x, c.y));
        if (iterator == chunks.end())
            return nullptr;

        return iterator->second;
    }
};

extern c_world* g_world;
