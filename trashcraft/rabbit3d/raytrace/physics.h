#pragma once
#include "scene.h"

class c_scene;
class c_player;
class c_entity;
class c_scene_object;

class c_physics
{
public:
    inline bool check_aabb_collision(const aabb_t& a, const aabb_t& b) {
        return (a.min.x < b.max.x && a.max.x > b.min.x) &&
            (a.min.y < b.max.y && a.max.y > b.min.y) &&
            (a.min.z < b.max.z && a.max.z > b.min.z);
    };
};

extern c_physics* g_physics;
