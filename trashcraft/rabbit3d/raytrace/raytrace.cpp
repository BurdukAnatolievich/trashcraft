#include "raytrace.h"
#include "scene.h"

#include <world.h>
#include <block.h>
#include <global_vars.h>

c_raytrace_utils* g_raytrace_utils = new c_raytrace_utils;

bool c_raytrace_utils::voxel_raycast(const glm::vec3& origin, const glm::vec3& direction, trace_t& trace, float max_distance)
{
    if (glm::length2(direction) < 0.0001f)
        return false;

    glm::vec3 dir = glm::normalize(direction);
    glm::ivec3 cur_block = glm::ivec3(glm::floor(origin));

    glm::ivec3 step = glm::ivec3(
        (dir.x >= 0.0f) ? 1 : -1,
        (dir.y >= 0.0f) ? 1 : -1,
        (dir.z >= 0.0f) ? 1 : -1
    );

    glm::vec3 safe_dir = glm::vec3(
        (std::abs(dir.x) < 0.00001f) ? 0.00001f : dir.x,
        (std::abs(dir.y) < 0.00001f) ? 0.00001f : dir.y,
        (std::abs(dir.z) < 0.00001f) ? 0.00001f : dir.z
    );

    glm::vec3 delta = glm::abs(1.0f / safe_dir);

    glm::vec3 t_max = glm::vec3(
        (dir.x >= 0.0f) ? (glm::floor(origin.x) + 1.0f - origin.x) * delta.x : (origin.x - glm::floor(origin.x)) * delta.x,
        (dir.y >= 0.0f) ? (glm::floor(origin.y) + 1.0f - origin.y) * delta.y : (origin.y - glm::floor(origin.y)) * delta.y,
        (dir.z >= 0.0f) ? (glm::floor(origin.z) + 1.0f - origin.z) * delta.z : (origin.z - glm::floor(origin.z)) * delta.z
    );

    glm::ivec3 last_normal = glm::ivec3(0);

    float current_dist = 0.0f;
    while (current_dist < max_distance) {
        if (cur_block.y < 0) {
            break;
        }

        uint32_t block = g_world->get_block_at_local_pos(cur_block);
        uint16_t block_id = block & 0x3FFF;
        if (block_id != 0 && block_id != 0x13371337) {
			c_block* block_ptr = (c_block*)g_global_vars->mapped_items[block_id];
            if (block_ptr->is_solid) {
                trace.hit = true;
                trace.block = block_id;
                trace.hit_pos = glm::vec3(cur_block);
                trace.normal = last_normal;
				trace.distance = current_dist;
                return true;
			}
        }

        if (t_max.x < t_max.y) {
            if (t_max.x < t_max.z) {
                current_dist = t_max.x;
                t_max.x += delta.x;
                cur_block.x += step.x;
                last_normal = glm::ivec3(-step.x, 0, 0);
            }
            else {
                current_dist = t_max.z;
                t_max.z += delta.z;
                cur_block.z += step.z;
                last_normal = glm::ivec3(0, 0, -step.z);
            }
        }
        else {
            if (t_max.y < t_max.z) {
                current_dist = t_max.y;
                t_max.y += delta.y;
                cur_block.y += step.y;
                last_normal = glm::ivec3(0, -step.y, 0);
            }
            else {
                current_dist = t_max.z;
                t_max.z += delta.z;
                cur_block.z += step.z;
                last_normal = glm::ivec3(0, 0, -step.z);
            }
        }
    }

    return false;
}