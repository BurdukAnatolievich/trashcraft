#pragma once
#include <glm.hpp>
#include <ext.hpp>

struct trace_t {
    bool hit = false;
    glm::ivec3 hit_pos = glm::ivec3(0);

    float distance = 0.0f;
    glm::ivec3 normal = glm::ivec3(0);

    uint32_t block = 0;
};

class c_raytrace_utils
{
public:
    bool voxel_raycast(const glm::vec3& origin, const glm::vec3& direction, trace_t& trace, float max_distance);
};

extern c_raytrace_utils* g_raytrace_utils;
