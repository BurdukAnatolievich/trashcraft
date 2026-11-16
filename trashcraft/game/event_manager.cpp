#include "event_manager.h"
#include "event_listeners.h"

c_event_manager* g_event_manager = new c_event_manager;

void c_event_manager::initialize()
{
    listeners["on_jukebox_disc_insert"].reserve(32);
    listeners["on_jukebox_disc_eject"].reserve(32);
    listeners["on_block_break"].reserve(32);
    listeners["on_player_death"].reserve(32);

    initialize_listeners();
}

void c_event_manager::initialize_listeners()
{
    bind_listener("on_jukebox_disc_insert", (event_listener_fn)c_event_listeners::on_jukebox_disc_insert);
    bind_listener("on_jukebox_disc_eject", (event_listener_fn)c_event_listeners::on_jukebox_disc_eject);

    bind_listener("on_block_break", (event_listener_fn)c_event_listeners::on_jukebox_block_break);

    bind_listener("on_player_death", (event_listener_fn)c_event_listeners::on_player_death_msg);
}

void c_event_manager::bind_listener(std::string event_name, event_listener_fn listener)
{
    listeners[event_name].push_back(listener);
}

void c_event_manager::remove_listener(std::string event_name, event_listener_fn listener_)
{
    std::vector<event_listener_fn>& event_listeners = listeners[event_name];
    for (int i = 0; i < event_listeners.size(); i++)
    {
        event_listener_fn& listener = event_listeners.at(i);
        if (listener == listener_)
        {
            event_listeners.erase(event_listeners.begin() + i);
            return;
        }
    }
}

void c_event_manager::remove_all_listeners()
{
    for (auto& i : listeners)
        i.second.clear();

    listeners.clear();
}

void c_event_manager::handle_listeners(std::string event_name, void* pass_argument)
{
    std::vector<event_listener_fn>& event_listeners = listeners[event_name];
    for (int i = 0; i < event_listeners.size(); i++)
    {
        event_listener_fn& listener = event_listeners.at(i);
        if (!listener)
            continue;

        listener(pass_argument);
    }
}
