#include "event_listeners.h"

#include "menu.h"
#include "item.h"
#include "block.h"

#include <audio/audio_manager.h>

void c_event_listeners::on_jukebox_disc_insert(c_jukebox* jukebox)
{
    //if (!jukebox)
    //    return;

    //if (!jukebox->block || jukebox->block->block_type != c_block::e_block_type::JUKEBOX)
    //    return;

    //c_item* disc = jukebox->disc.item_data.item;
    //if (!disc)
    //    return;

    //if (disc->type != c_item::e_type::RECORD)
    //    return;

    //c_music_disc* disc_ = (c_music_disc*)disc;
    //jukebox->current_channel = g_audio_manager->play_music(disc_->music_id, false, true);
}

void c_event_listeners::on_jukebox_disc_eject(c_jukebox* jukebox)
{
    //if (!jukebox)
    //    return;

    //if (!jukebox->block || jukebox->block->block_type != c_block::e_block_type::JUKEBOX)
    //    return;

    //if (jukebox->current_channel != c_audio_manager::e_channels::INVALID)
    //{
    //    g_audio_manager->stop_music(jukebox->current_channel);
    //    jukebox->current_channel = c_audio_manager::e_channels::INVALID;
    //}
}

void c_event_listeners::on_jukebox_block_break(c_jukebox* jukebox)
{
    //if (!jukebox)
    //    return;

    //if (!jukebox->block || jukebox->block->block_type != c_block::e_block_type::JUKEBOX)
    //    return;

    //if (jukebox->has_disc)
    //{
    //    c_inventory_item* inv_item = g_global_vars->give_item(jukebox->disc.item_data.item);
    //    if (inv_item)
    //    {
    //        std::memcpy(inv_item, &jukebox->disc, sizeof(c_inventory_item));

    //        jukebox->has_disc = false;
    //        std::memset(&jukebox->disc, 0, sizeof(c_inventory_item));
    //    }
    //}

    //on_jukebox_disc_eject(jukebox);
}

void c_event_listeners::on_player_death_msg(c_player::player_death_event_t* event_data)
{
    if (!event_data)
        return;

    if (!event_data->player || event_data->player != g_global_vars->local_player)
        return;

    std::string text_reason = "";
    switch (event_data->reason)
    {
    case c_player::e_damage_reason::UNKNOWN:
        text_reason = " died. Why? No one knows.";
        break;
    case c_player::e_damage_reason::FALL_DAMAGE:
        text_reason = " fell from a high place.";
        break;
    }

    std::string translated_reason = event_data->player->name + text_reason;
    
}

void c_event_listeners::on_player_respawn_msg(c_player* player)
{
    if (!player)
        return;

    if (player != g_global_vars->local_player)
        return;

    
}
