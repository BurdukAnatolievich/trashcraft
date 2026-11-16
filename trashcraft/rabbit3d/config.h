#pragma once
#include <glm.hpp>

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
