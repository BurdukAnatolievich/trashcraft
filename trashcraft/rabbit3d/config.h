#pragma once
#include <glm.hpp>

constexpr int TRASHCRAFT_MAJOR_VERSION = 0;
constexpr int TRASHCRAFT_MINOR_VERSION = 28;
constexpr const char* TRASHCRAFT_ITERATION = "Infinite";
constexpr const char* TRASHCRAFT_VERSION_STRING = "0.28 Infinite";
constexpr int TRASHCRAFT_WORLD_GENERATOR_VERSION = 2;

constexpr int WORLD_HEIGHT = 256;
constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_SIZE_SHIFT = 4;
constexpr int MAX_RENDER_DIST = 16;
constexpr int MAX_RENDER_DIST_R = MAX_RENDER_DIST / 2;

constexpr int INVENTORY_BAR_COUNT = 3;
constexpr int INVENTORY_BAR_SIZE = 9;

class c_rabbit3d_config
{
    glm::vec2 screen_resolution = glm::vec2(0.0f);
public:
    struct audio_t
    {
        int music_volume = 100;
        int jukebox_volume = 100;
        int sound_volume = 100;
    } audio;

    struct render_t
    {
        int field_of_view = 70;

        int render_resolution = 0;
        int render_distance = MAX_RENDER_DIST_R;
    } render;

    void initialize();

    inline glm::vec2 get_screen_resolution() { return screen_resolution; };
    inline float get_aspect_ratio() { return (screen_resolution.x / screen_resolution.y); };

};

extern c_rabbit3d_config* g_config;
