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

    new c_block("Oak planks", "oak_planks", { "wood", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY);
    new c_block("Oak leaves", "oak_leaves", { "leaves", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, c_tool::e_tool::ANY, c_tool::e_tool::ANY, "", true, true);
    new c_block("Oak log", "oak_log", { "tree_side", "tree_top", "tree_top" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY);

    new c_block("Glass", "glass", { "glass", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1, c_tool::e_tool::ANY, c_tool::e_tool::ANY, "", true, true);

    new c_block("Dirt", "dirt", { "dirt", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f, c_tool::e_tool::ANY | c_tool::e_tool::SHOVEL, c_tool::e_tool::ANY);
    new c_block("Grass", "grass_block", { "grass_side", "grass_top", "dirt" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.6f, c_tool::e_tool::ANY | c_tool::e_tool::SHOVEL, c_tool::e_tool::ANY, "dirt");

    new c_block("Stone", "stone", { "stone", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.5f, c_tool::e_tool::PICKAXE, c_tool::e_tool::PICKAXE);

    new c_block("Crafting table", "crafting_table", { "workbench_side", "workbench_top", "wood" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.5f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_DEFAULT, "", c_block::e_interactable_type::I_CRAFTING_TABLE);
    new c_block("Jukebox", "jukebox", { "musicBlock", "jukebox_top", "musicBlock" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_JUKEBOX, "", c_block::e_interactable_type::I_JUKEBOX);

    new c_block("Chest", "chest", { "chest_side", "chest_top", "chest_top" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.5f, c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_CHEST, "", c_block::e_interactable_type::I_CHEST);

    new c_block("Sand", "sand", { "sand", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f, c_tool::e_tool::ANY | c_tool::e_tool::SHOVEL, c_tool::e_tool::ANY);
    new c_block("Water", "water", { "water", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.875f, 1.0f), -1.0f, c_tool::e_tool::NONE, c_tool::e_tool::NONE, "", false, true);

    new c_block("Oak door", "oak_door", { "", "", "" }, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1875f, 1.0f, 2.0f), 3.0f, c_tool::e_tool::ANY | c_tool::e_tool::AXE, c_tool::e_tool::ANY, "", true, false, false, c_block::e_block_type::T_DOOR, "cherry_door", c_block::e_interactable_type::I_DOOR, "cherry_door_block");
}

void c_global_vars::initialize_items()
{
    // ITEMS

    new c_item("Stick", "stick", "stick");
    new c_item("Iron ingot", "iron_ingot", "ingotIron");

    new c_item("Diamond", "diamond", "diamond");

    // TOOLS

    new c_tool("Iron pickaxe", "iron_pickaxe", c_tool::e_tool::PICKAXE, 250, 6, "pickaxeIron");

    // RECORDS

    new c_music_disc("Music Disc", "music_disc_chirp", "C418 - Chirp", "record_chirp", "music_disc_chirp");
    
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

void c_global_vars::start()
{
    g_event_manager->initialize_listeners();

    g_global_vars->local_player = new c_player;

	c_player* player = g_global_vars->local_player;
    player->bind_camera(camera);
    player->set_model(g_model_manager->get_model_instance("local_mdl"));

    std::memcpy(player->name, "Steve", sizeof("Steve"));
    player->respawn();

	player->inventory.items[0] = { g_global_vars->items["oak_planks"]->numeric_id, 64 };

    g_entity_list->add_entity(player);
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