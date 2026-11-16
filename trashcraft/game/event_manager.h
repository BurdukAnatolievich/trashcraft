#pragma once
#include <unordered_map>
#include <vector>
#include <string>

class c_event_manager
{
    using event_listener_fn = void(*)(void*);
    std::unordered_map<std::string, std::vector<event_listener_fn>> listeners;
public:
    void initialize();
    void initialize_listeners();

    void bind_listener(std::string event_name, event_listener_fn listener);
    void remove_listener(std::string event_name, event_listener_fn listener);

    void remove_all_listeners();

    void handle_listeners(std::string event_name, void* pass_argument = nullptr);
};

extern c_event_manager* g_event_manager;

