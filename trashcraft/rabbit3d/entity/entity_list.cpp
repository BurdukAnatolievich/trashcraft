#include "entity_list.h"

c_entity_list* g_entity_list = new c_entity_list;

void c_entity_list::add_entity(c_entity* entity)
{
    if (!entity) 
        return;

    std::lock_guard<std::mutex> lock(entity_mutex);
    entities.push_back(entity);
}

c_entity* c_entity_list::get_entity(int idx)
{
    if (entities.empty())
        return nullptr;

    if (idx < 0 || idx >= static_cast<int>(entities.size()))
        return nullptr;

    return entities.at(idx);
}

int c_entity_list::get_entity_count()
{
    return static_cast<int>(entities.size());
}

void c_entity_list::remove_entity(c_entity* entity)
{
    if (!entity) 
        return;

    std::lock_guard<std::mutex> lock(entity_mutex);
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        if (*it == entity) {
            switch (entity->type)
            {
            case c_entity::e_type::PLAYER:
                delete (static_cast<c_player*>(entity));
                break;
            case c_entity::e_type::DROPPED_ITEM:
                delete (static_cast<c_dropped_item*>(entity));
                break;
            default:
                delete entity;
                break;
            }

            entities.erase(it);
            break;
        }
    }
}

void c_entity_list::remove_entity(int idx)
{
    std::lock_guard<std::mutex> lock(entity_mutex);
    if (idx < 0 || idx >= static_cast<int>(entities.size()))
        return;

    c_entity* entity = entities.at(idx);
    if (entity) {
        switch (entity->type)
        {
        case c_entity::e_type::PLAYER:
            delete (static_cast<c_player*>(entity));
            break;
        case c_entity::e_type::DROPPED_ITEM:
            delete (static_cast<c_dropped_item*>(entity));
            break;
        default:
            delete entity;
            break;
        }
    }

    entities.erase(entities.begin() + idx);
}

void c_entity_list::remove_all_entities()
{
    std::lock_guard<std::mutex> lock(entity_mutex);
    for (c_entity* entity : entities) {
        if (!entity) 
            continue;

        switch (entity->type)
        {
        case c_entity::e_type::PLAYER:
            delete (static_cast<c_player*>(entity));
            break;
        case c_entity::e_type::DROPPED_ITEM:
            delete (static_cast<c_dropped_item*>(entity));
            break;
        default:
            delete entity;
            break;
        }
    }

    entities.clear();
}