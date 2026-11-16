#pragma once
#include "entity/entity_list.h"

class c_jukebox;
class c_block_data;

class c_event_listeners
{
public:
    static void on_jukebox_disc_insert(c_jukebox* jukebox);
    static void on_jukebox_disc_eject(c_jukebox* jukebox);
    static void on_jukebox_block_break(c_jukebox* jukebox);

    static void on_player_death_msg(c_player::player_death_event_t* event_data);
    static void on_player_respawn_msg(c_player* player);
};
