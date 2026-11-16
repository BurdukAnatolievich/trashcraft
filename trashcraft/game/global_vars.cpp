#include "global_vars.h"
#include "block.h"

#include <rabbit3d.h>
#include "menu.h"
#include <audio/audio_manager.h>
#include "event_manager.h"

c_global_vars* g_global_vars = new c_global_vars;

void c_global_vars::initialize_blocks()
{
    // BLOCKS

    new c_block("Air", "air", { "", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), -1.0f);

    new c_block("Cherry planks", "cherry_planks", { "cherry_planks", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY);
    new c_block("Cherry leaves", "cherry_leaves", { "cherry_leaves", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, c_tool::e_tool::ANY, c_tool::e_tool::ANY, "", true, true);
    new c_block("Cherry log", "cherry_log", { "cherry_log", "cherry_log_top", "cherry_log_top" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY);

    new c_block("Glass", "glass", { "glass", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1, c_tool::e_tool::ANY, c_tool::e_tool::ANY, "", true, true);

    new c_block("Dirt", "dirt", { "dirt", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f, c_tool::e_tool::ANY | c_tool::e_tool::SHOVEL, c_tool::e_tool::ANY);
    new c_block("Grass", "grass_block", { "grass_block_side", "grass_block_top", "dirt" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.6f, c_tool::e_tool::ANY | c_tool::e_tool::SHOVEL, c_tool::e_tool::ANY, "dirt");

    new c_block("Stone", "stone", { "stone", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.5f, c_tool::e_tool::PICKAXE, c_tool::e_tool::PICKAXE);

    new c_block("Crafting table", "crafting_table", { "crafting_table_side", "crafting_table_top", "oak_planks" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.5f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_DEFAULT, "", c_block::e_interactable_type::I_CRAFTING_TABLE);
    new c_block("Jukebox", "jukebox", { "jukebox_side", "jukebox_top", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_JUKEBOX, "", c_block::e_interactable_type::I_JUKEBOX);

    new c_block("Chest", "chest", { "chest_side", "chest_top", "chest_top" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.5f, c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_CHEST, "", c_block::e_interactable_type::I_CHEST);

    new c_block("Sand", "sand", { "sand", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f, c_tool::e_tool::ANY | c_tool::e_tool::SHOVEL, c_tool::e_tool::ANY);
    new c_block("Water", "water", { "water_still", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.875f, 1.0f), -1.0f, c_tool::e_tool::NONE, c_tool::e_tool::NONE, "", false, true);

    new c_block("Cherry door", "cherry_door", { "", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1875f, 1.0f, 2.0f), 3.0f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_DOOR, "cherry_door", c_block::e_interactable_type::I_DOOR, "cherry_door_block");
}

void c_global_vars::initialize_items()
{
    // ITEMS

    new c_item("Stick", "stick", "stick");
    new c_item("Iron ingot", "iron_ingot", "iron_ingot");

    new c_item("Diamond", "diamond", "diamond");

    // TOOLS

    new c_tool("Iron pickaxe", "iron_pickaxe", c_tool::e_tool::PICKAXE, 250, 6, "iron_pickaxe");

    // RECORDS

    new c_music_disc("Music Disc", "music_disc_chirp", "C418 - Chirp", "record_chirp", "music_disc_chirp");
    new c_music_disc("Music Disc", "music_disc_cherry", "ssshhhiiittt! - vishenka", "record_cherry", "music_disc_chirp");
    new c_music_disc("Music Disc", "music_disc_tam", "bilbordy - tam", "record_tam", "music_disc_tam");
}

void c_global_vars::initialize_recipes()
{
    // CRAFTING RECIPES

    new c_crafting_recipe("", "cherry_planks_recipe",
        { { "cherry_log" } }, "cherry_planks", 4);

    new c_crafting_recipe("", "stick_recipe",
        { { "cherry_planks" },
          { "cherry_planks" } }, "stick", 4);

    new c_crafting_recipe("", "crafting_table_recipe",
        { { "cherry_planks", "cherry_planks" },
          { "cherry_planks", "cherry_planks" } }, "crafting_table", 1);

    new c_crafting_recipe("", "chest_recipe",
        { { "cherry_planks", "cherry_planks", "cherry_planks" },
          { "cherry_planks", "", "cherry_planks" },
          { "cherry_planks", "cherry_planks", "cherry_planks" } }, "chest");

    new c_crafting_recipe("", "iron_pickaxe_recipe",
        { { "iron_ingot", "iron_ingot", "iron_ingot" },
          { "", "stick", "" },
          { "", "stick", "" } }, "iron_pickaxe");

    new c_crafting_recipe("", "chest_recipe",
        { { "cherry_planks", "cherry_planks" },
          { "cherry_planks", "cherry_planks" },
          { "cherry_planks", "cherry_planks" } }, "cherry_door");
}

c_inventory_item* c_global_vars::give_item(c_item* item, int count)
{
    c_inventory_item* free_inventory_slot = nullptr;
    if (count > item->max_count)
        return free_inventory_slot;

    // scan for inventory slot with same item type
    for (int i = e_inventory_bar::BAR_HOTBAR; i != e_inventory_bar::BAR_INVALID; i--)
    {
        c_inventory_item* inventory_bar = g_global_vars->inventory_items[i];
        for (int c = 0; c < 9; c++)
        {
            c_inventory_item* check_slot = &inventory_bar[c];
            if (check_slot == ignore_inventory_slot)
                continue;

            if (check_slot->item_data.durability == -1 && check_slot->item_data.item == item && check_slot->count && (check_slot->count + count) < check_slot->item_data.item->max_count)
            {
                free_inventory_slot = check_slot;
                break;
            }
        }

        if (free_inventory_slot)
            break;
    }

    // scan for free inventory slot if we didnt found one with same item type
    if (!free_inventory_slot)
    {
        for (int i = e_inventory_bar::BAR_HOTBAR; i != e_inventory_bar::BAR_INVALID; i--)
        {
            c_inventory_item* inventory_bar = g_global_vars->inventory_items[i];
            for (int c = 0; c < 9; c++)
            {
                c_inventory_item* check_slot = &inventory_bar[c];
                if (check_slot == ignore_inventory_slot)
                    continue;

                if (!check_slot->item_data.item && !check_slot->count)
                {
                    free_inventory_slot = check_slot;
                    break;
                }
            }

            if (free_inventory_slot)
                break;
        }
    }

    if (free_inventory_slot)
    {
        free_inventory_slot->item_data.item = item;
        free_inventory_slot->count += count;
    }

    return free_inventory_slot;
}

void c_global_vars::drop_item(c_inventory_item* item, glm::vec3 origin, glm::vec3 up)
{
    c_dropped_item* dropped_item = new c_dropped_item();
    g_entity_list->add_entity(dropped_item);

    std::memcpy(&dropped_item->item, item, sizeof(c_inventory_item));

    item->count = 0;
    item->item_data.reset();

    dropped_item->can_be_picked_up = false;

    dropped_item->set_origin(origin.x != FLT_MAX ? origin : local_player->eye_position);
    dropped_item->velocity = up.x != FLT_MAX ? up * 0.1f : camera->get_forward() * 0.1f;
}

bool c_global_vars::put_item(c_inventory_item* item)
{
    c_inventory_item* free_inventory_slot = nullptr;

    int items_left = 0;
    int items_taken = 0;

    // scan for inventory slot with same item type
    for (int i = e_inventory_bar::BAR_HOTBAR; i != e_inventory_bar::BAR_INVALID; i--)
    {
        c_inventory_item* inventory_bar = g_global_vars->inventory_items[i];
        for (int c = 0; c < 9; c++)
        {
            c_inventory_item* check_slot = &inventory_bar[c];
            if (check_slot == ignore_inventory_slot)
                continue;

            if (check_slot->item_data.durability == -1 && item->item_data.durability == -1 && check_slot->item_data.item == item->item_data.item && check_slot->count && check_slot->count < 64)
            {
                free_inventory_slot = check_slot;

                items_left = (check_slot->count + item->count) - check_slot->item_data.item->max_count;
                items_taken = item->count - std::clamp(items_left, 0, check_slot->item_data.item->max_count);

                break;
            }
        }

        if (free_inventory_slot)
            break;
    }

    // scan for free inventory slot if we didnt found one with same item type
    if (!free_inventory_slot)
    {
        for (int i = e_inventory_bar::BAR_HOTBAR; i != e_inventory_bar::BAR_INVALID; i--)
        {
            c_inventory_item* inventory_bar = g_global_vars->inventory_items[i];
            for (int c = 0; c < 9; c++)
            {
                c_inventory_item* check_slot = &inventory_bar[c];
                if (check_slot == ignore_inventory_slot)
                    continue;

                if (!check_slot->item_data.item && !check_slot->count)
                {
                    free_inventory_slot = check_slot;

                    items_left = 0;
                    items_taken = item->count;
                    break;
                }
            }

            if (free_inventory_slot)
                break;
        }
    }

    if (free_inventory_slot)
    {
        free_inventory_slot->item_data = item->item_data;
        free_inventory_slot->count += items_taken;

        if (items_left <= 0)
            std::memset(item, 0, sizeof(c_inventory_item));
        else
            item->count = items_left;

        return true;
    }

    return false;
}

void c_global_vars::start()
{
    g_event_manager->initialize_listeners();

    g_global_vars->local_player = new c_player;
    g_global_vars->local_player->bind_camera(camera);

    c_model* mdl = g_model_manager->find_model_by_name("local_mdl");
    g_global_vars->local_player->set_model(mdl);

    g_global_vars->local_player->name = "Notch";
    g_global_vars->local_player->respawn();

    g_entity_list->add_entity(g_global_vars->local_player);
}

void c_global_vars::finish()
{
    g_audio_manager->close_all_channels();
    g_event_manager->remove_all_listeners();

    g_entity_list->remove_all_entities();

    g_global_vars->local_player = nullptr;
}

void c_global_vars::map_items()
{
    std::memset(mapped_items, 0, sizeof(mapped_items));
    std::memset(mapped_transparency, 0, sizeof(mapped_transparency));

    for (auto& item_ : items) {
        c_item* item = item_.second;
        c_block* block = (c_block*)item;

        mapped_items[item->numeric_id] = item;
        mapped_transparency[item->numeric_id] = item->type == c_item::e_type::BLOCK ? (block->is_transparent || !block->is_solid) : false;
    }
}