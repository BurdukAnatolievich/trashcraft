#include "game.h"
#include <rabbit3d.h>
#include <audio/audio_manager.h>
#include <gui/main_menu.h>

c_game* g_game = new c_game;

void c_game::initialize()
{
    g_menu_draw->initialize();
    g_ui_manager->push_screen(new c_main_menu());

    g_time->set_interval_per_tick(1.0 / 20.0);

    g_global_vars->scene = new c_scene;
    g_global_vars->camera = new c_camera;

    g_global_vars->in_game = false;

    initialize_render();
    initialize_scene();

    g_global_vars->initialize_blocks();
    g_global_vars->initialize_items();
    g_global_vars->initialize_recipes();

    g_global_vars->map_items();

    g_audio_manager->initialize_audio();
    g_audio_manager->initialize_sounds();

    g_world->initialize_structures();
    g_world_save->initialize();

    g_input->initialize();
}

void c_game::initialize_render()
{
    g_global_vars->camera->set_field_of_view(g_config->render.field_of_view);
    g_global_vars->camera->set_aspect_ratio(g_config->get_aspect_ratio());

    g_global_vars->scene->set_skybox(new c_skybox());
    g_global_vars->scene->get_skybox()->set_sun_position(glm::normalize(glm::vec3(-0.5f, 0.75f, -1.0f)) * 384.0f);

    g_render->initialize();
    g_texture_manager->initialize();
    g_model_manager->initialize();
}

void c_game::initialize_scene()
{
    g_render->set_scene(g_global_vars->scene);
    g_render->set_camera(g_global_vars->camera);

    g_render->set_active(false);
}

void c_game::start()
{
    g_time->start();
    g_global_vars->start();

    g_render->set_active(true);

    g_input->unlock();

    g_world->set_generator_seed(12345);

    if (g_global_vars->world_id != -1)
        g_world_save->load_world(g_global_vars->world_id);
    else
        g_world_save->create_world(g_global_vars->world_name, g_global_vars->world_name);

    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][0].count = 32;
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][0].item_data.reset();
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][0].item_data.item = g_global_vars->items["glass"];

    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][1].count = 20;
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][1].item_data.reset();
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][1].item_data.item = g_global_vars->items["jukebox"];

    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][2].count = 3;
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][2].item_data.reset();
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][2].item_data.item = g_global_vars->items["diamond"];

    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][3].count = 1;
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][3].item_data.reset();
    g_global_vars->inventory_items[e_inventory_bar::BAR_HOTBAR][3].item_data.item = g_global_vars->items["iron_pickaxe"];


    g_global_vars->in_game = true;
}

void c_game::update()
{
    if (!g_global_vars->in_game) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));
        return;
    }

    g_world->generate_world_based_on_player_position();

    g_time->update();

    if (!g_time->can_run_update()) {
        //g_time->sleep_update();
        return;
    }

    g_audio_manager->update_channels();

    {
        //std::lock_guard<std::mutex> lock(g_entity_list->entity_mutex);

        for (int i = g_entity_list->get_entity_count() - 1; i >= 0; i--) {
            c_entity* entity = g_entity_list->get_entity(i);
            if (!entity) 
                continue;

            if (entity->should_be_destroyed) {
                g_entity_list->remove_entity(i);
                continue;
            }

            switch (entity->type)
            {
            case c_entity::e_type::PLAYER:
                static_cast<c_player*>(entity)->update();
                break;
            case c_entity::e_type::DROPPED_ITEM:
                static_cast<c_dropped_item*>(entity)->update();
                break;
            }
        }
    }

    g_world->update_chunks(true, true);

    g_time->ran_update();
    //g_time->sleep_update();
}

void c_game::finish()
{
    g_global_vars->in_game = false;

    g_render->set_active(false);

    g_world->clear_world();

    g_global_vars->current_inventory_slot = 0;
    std::memset(g_global_vars->inventory_items, 0, sizeof(c_inventory_item) * 4 * 9);

    g_time->finish();
    g_global_vars->finish();
}
