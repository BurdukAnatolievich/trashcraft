#include "audio_manager.h"
#include <config.h>

c_audio_manager* g_audio_manager = new c_audio_manager;

void c_audio_manager::channel_t::create(std::string path, glm::vec3 origin)
{
    if (stream)
        close();

    volume = 1.0f;

    stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, 0);
    paused = false;

    this->origin = origin;
}

void c_audio_manager::channel_t::play()
{
    BASS_ChannelPlay(stream, !paused);
    paused = false;
}

void c_audio_manager::channel_t::pause()
{
    BASS_ChannelPause(stream);
    paused = true;
}

void c_audio_manager::channel_t::stop()
{
    BASS_ChannelPause(stream);
    paused = false;
}

void c_audio_manager::channel_t::close()
{
    BASS_ChannelStop(stream);
    BASS_ChannelFree(stream);

    paused = false;
    stream = NULL;

    origin = glm::vec3(FLT_MAX);
    volume = 0.0f;
}

void c_audio_manager::channel_t::set_volume(float volume_)
{
    volume = volume_;
    BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
}

bool c_audio_manager::channel_t::is_playing()
{
    return BASS_ChannelIsActive(stream) != BASS_ACTIVE_STOPPED;
}

void c_audio_manager::initialize_audio()
{
    BASS_Init(-1, 44100, 0, 0, NULL);
}

void c_audio_manager::initialize_sounds()
{
    cache_audio_file("record_chirp", "./sounds/records/chirp.mp3");
    cache_audio_file("record_cherry", "./sounds/records/cherry.mp3");
    cache_audio_file("record_tam", "./sounds/records/tam.mp3");
}

void c_audio_manager::update_channels()
{
    for (int i = e_channels::MAX; i < channels.size(); i++)
    {
        channel_t* channel = &channels.at(i);
        if (!channel)
            continue;

        if (!channel->is_playing())
        {
            channel->close();

            channels.erase(channels.begin() + i);
            continue;
        }

        if (channel->category == e_categories::JUKEBOX)
            channel->set_volume((float)g_config->audio.jukebox_volume / 100.0f);

        if (channel->category == e_categories::MUSIC)
            channel->set_volume((float)g_config->audio.music_volume / 100.0f);

        if (channel->category == e_categories::SOUND)
            channel->set_volume((float)g_config->audio.sound_volume / 100.0f);
    }
}

void c_audio_manager::close_all_channels()
{
    for (int i = 0; i < channels.size(); i++)
    {
        channel_t* channel = &channels.at(i);
        if (!channel)
            continue;

        if (i >= e_channels::PLAYER_SOUND)
        {
            channel->close();

            if (i >= e_channels::MAX)
                channels.erase(channels.begin() + i);
        }
    }
}

void c_audio_manager::cache_audio_file(std::string id, std::string path)
{
    cached_audio_files[id] = path;
}

void c_audio_manager::play_sound(std::string id, bool separate_channel)
{
    std::string sound_file = cached_audio_files[id];

    HSTREAM stream = BASS_StreamCreateFile(FALSE, sound_file.c_str(), 0, 0, 0);
    if (separate_channel)
    {
        int id_ = channels.size();

        channel_t& channel = channels.emplace_back();
        channel.id = id_;
        channel.category = e_categories::SOUND;

        channel.create(sound_file);
        channel.play();
    }
    else
    {
        channel_t& channel = channels[e_channels::PLAYER_SOUND];

        channel.create(sound_file);
        channel.play();
    }
}

int c_audio_manager::play_music(std::string id, bool ambient, bool jukebox, glm::vec3 sound_origin)
{
    std::string music_file = cached_audio_files[id];

    HSTREAM stream = BASS_StreamCreateFile(FALSE, music_file.c_str(), 0, 0, 0);
    if (ambient)
    {
        channel_t& channel = channels[e_channels::AMBIENT_MUSIC];

        channel.create(music_file);
        channel.play();

        return e_channels::AMBIENT_MUSIC;
    }
    else
    {
        int id_ = channels.size();

        channel_t& channel = channels.emplace_back();
        channel.id = id_;
        channel.category = jukebox ? e_categories::JUKEBOX : e_categories::MUSIC;

        channel.create(music_file, sound_origin);
        channel.play();

        return channel.id;
    }

    return e_channels::INVALID;
}

void c_audio_manager::stop_music(int channel_id)
{
    for (int i = 0; i < channels.size(); i++)
    {
        channel_t* channel = &channels.at(i);
        if (!channel)
            continue;

        if (channel->id != channel_id)
            continue;

        channel->close();

        channels.erase(channels.begin() + i);
        break;
    }
}

bool c_audio_manager::channel_exists(int channel_id)
{
    for (int i = 0; i < channels.size(); i++)
    {
        channel_t* channel = &channels.at(i);
        if (!channel)
            continue;

        if (channel->id == channel_id)
            return true;
    }

    return false;
}
