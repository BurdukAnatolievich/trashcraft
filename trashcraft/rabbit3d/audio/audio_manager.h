#pragma once
#include "bass.h"

#include <string>
#include <deque>

#include <unordered_map>

#include <glm.hpp>

class c_audio_manager
{
public:
    enum e_channels
    {
        INVALID = -1,
        AMBIENT_MUSIC,
        PLAYER_SOUND,
        MAX
    };

    enum e_categories
    {
        NONE = -1,
        MUSIC,
        SOUND,
        JUKEBOX
    };

    struct channel_t
    {
        int id = e_channels::INVALID;
        int category = e_categories::NONE;

        HSTREAM stream = NULL;

        bool paused = false;
        glm::vec3 origin = glm::vec3(FLT_MAX);

        float volume = 1.0f;

        void create(std::string path, glm::vec3 origin = glm::vec3(FLT_MAX));

        void play();
        void pause();

        void stop();

        void close();

        void set_volume(float volume);
        bool is_playing();
    };
private:
    int last_music_volume = 100;
    int last_jukebox_volume = 100;
    int last_sound_volume = 100;

    std::unordered_map<std::string, std::string> cached_audio_files;

    std::deque<channel_t> channels = {
        { e_channels::AMBIENT_MUSIC, e_categories::MUSIC, NULL, false, glm::vec3(FLT_MAX), 1.0f },
        { e_channels::PLAYER_SOUND, e_categories::SOUND, NULL, false, glm::vec3(FLT_MAX), 1.0f },
    };
public:
    void initialize_audio();
    void initialize_sounds();

    void update_channels();
    void close_all_channels();

    void cache_audio_file(std::string id, std::string path);

    void play_sound(std::string id, bool separate_channel = false);

    int play_music(std::string id, bool ambient = true, bool jukebox = false, glm::vec3 sound_origin = glm::vec3(FLT_MAX));
    void stop_music(int channel);

    bool channel_exists(int channel);
};

extern c_audio_manager* g_audio_manager;
