#include "item.h"
#include "menu.h"
#include "global_vars.h"

#include <raytrace/texture.h>
#include <raytrace/models.h>

#include <algorithm>

c_item::c_item()
{
    this->type = c_item::e_type::ITEM;
}

c_item::c_item(std::string register_name, std::string register_id, int max_count_)
{
    this->id = register_id;
    this->name = register_name;

    this->numeric_id = g_global_vars->items.size();

    this->type = c_item::e_type::ITEM;

    this->max_count = max_count_;

    g_global_vars->items[register_id] = this;
}

c_item::c_item(std::string register_name, std::string register_id, std::string material, int max_count_)
{
    this->id = register_id;
    this->name = register_name;

    this->numeric_id = g_global_vars->items.size();

    this->type = c_item::e_type::ITEM;
    this->max_count = max_count_;

    this->item_material = g_texture_manager->find_texture_by_name(material);
    this->item_model = g_model_manager->find_model_by_name(material);

    g_global_vars->items[register_id] = this;
}

c_tool::c_tool()
{
    this->type = c_item::e_type::TOOL;
}

c_tool::c_tool(std::string register_name, std::string register_id, int tool_type, int tool_durability, int tool_efficiency)
{
    this->id = register_id;
    this->name = register_name;

    this->numeric_id = g_global_vars->items.size();

    this->type = c_item::e_type::TOOL;

    this->tool = tool_type;

    this->durability = tool_durability;
    this->efficiency = tool_efficiency;

    this->max_count = 1;

    g_global_vars->items[register_id] = this;
}

c_tool::c_tool(std::string register_name, std::string register_id, int tool_type, int tool_durability, int tool_efficiency, std::string material)
{
    this->id = register_id;
    this->name = register_name;

    this->numeric_id = g_global_vars->items.size();

    this->type = c_item::e_type::TOOL;

    this->tool = tool_type;

    this->durability = tool_durability;
    this->efficiency = tool_efficiency;

    this->item_material = g_texture_manager->find_texture_by_name(material);
    this->item_model = g_model_manager->find_model_by_name(material);

    this->max_count = 1;

    g_global_vars->items[register_id] = this;
}

c_music_disc::c_music_disc()
{
    type = c_item::e_type::RECORD;
}

c_music_disc::c_music_disc(std::string register_name, std::string register_id, std::string register_desc, std::string music_id)
{
    this->id = register_id;
    this->name = register_name;
    this->description = register_desc;
    this->numeric_id = g_global_vars->items.size();

    this->type = c_item::e_type::RECORD;

    this->max_count = 1;

    std::memset(this->music_id, 0, sizeof(this->music_id));
    std::memcpy(this->music_id, music_id.data(), std::min(music_id.size(), sizeof(this->music_id) - 1));

    g_global_vars->items[register_id] = this;
}

c_music_disc::c_music_disc(std::string register_name, std::string register_id, std::string register_desc, std::string music_id, std::string material)
{
    this->id = register_id;
    this->name = register_name;
    this->description = register_desc;
    this->numeric_id = g_global_vars->items.size();

    this->type = c_item::e_type::RECORD;

    this->item_material = g_texture_manager->find_texture_by_name(material);
    this->item_model = g_model_manager->find_model_by_name(material);

    this->max_count = 1;

    std::memset(this->music_id, 0, sizeof(this->music_id));
    std::memcpy(this->music_id, music_id.data(), std::min(music_id.size(), sizeof(this->music_id) - 1));

    g_global_vars->items[register_id] = this;
}

c_crafting_recipe::c_crafting_recipe()
{

}

c_crafting_recipe::c_crafting_recipe(std::string register_name, std::string register_id, const std::vector<std::vector<std::string>>& recipe_, std::string product_item_id_, int product_item_count_)
{
    this->id = register_id;
    this->name = register_name;

    this->numeric_id = g_global_vars->recipes.size();

    this->recipe = recipe_;

    this->product_item_id = product_item_id_;
    this->product_item_count = product_item_count_;

    g_global_vars->recipes[register_id] = this;
}

glm::vec2 c_crafting_recipe::get_size()
{
    int size_x = 0;
    int size_y = 0;

    bool hx1 = false, hx2 = false, hx3 = false;
    bool hy1 = false, hy2 = false, hy3 = false;

    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            if (recipe.size() <= y)
                continue;

            if (recipe.at(y).size() <= x)
                continue;

            if (recipe[y][x] != "")
            {
                if (x == 0 && !hx1)
                {
                    size_x++;
                    hx1 = true;
                }
                else if (x == 1 && !hx2)
                {
                    size_x++;
                    hx2 = true;
                }
                else if (x == 2 && !hx3)
                {
                    size_x++;
                    hx3 = true;
                }

                if (y == 0 && !hy1)
                {
                    size_y++;
                    hy1 = true;
                }
                else if (y == 1 && !hy2)
                {
                    size_y++;
                    hy2 = true;
                }
                else if (y == 2 && !hy3)
                {
                    size_y++;
                    hy3 = true;
                }
            }
        }
    }

    return glm::vec2(size_x, size_y);
}

c_crafting_recipe::e_grid_type c_crafting_recipe::get_grid_type()
{
    glm::vec2 size = get_size();
    if (size.x <= 2 && size.y <= 2)
        return c_crafting_recipe::e_grid_type::GRID_2X2;

    return c_crafting_recipe::e_grid_type::GRID_3X3;
}

bool c_crafting_recipe::check_crafting_recipe(c_crafting_recipe::e_grid_type grid, glm::vec2 size, glm::vec2 position)
{
    int grid_x = grid == c_crafting_recipe::e_grid_type::GRID_2X2 ? 2 : 3;
    int grid_y = grid == c_crafting_recipe::e_grid_type::GRID_2X2 ? 2 : 3;

    int pos_clamped_x = std::clamp((int)position.x, 0, (int)(grid_x - size.x));
    int pos_clamped_y = std::clamp((int)position.y, 0, (int)(grid_y - size.y));

    int needed_matches = 0;
    //int needed_empty_slots = 0;

    int found_matches = 0;
    //int found_empty_slots = 0;

    // scan recipe
    for (int y = 0; y < recipe.size(); y++)
    {
        std::vector<std::string>& recipe_bar = recipe.at(y);
        for (int x = 0; x < recipe_bar.size(); x++)
        {
            if (recipe_bar.at(x) != "")
                needed_matches++;
        }
    }

    //needed_empty_slots = grid_x * grid_y - needed_matches;

    // scan grid and do matching
    //for (int y = 0; y < grid_y; y++)
    //{
    //    for (int x = 0; x < grid_x; x++)
    //    {
    //        int real_x = x - pos_clamped_x;
    //        int real_y = y - pos_clamped_y;

    //        c_inventory_item* grid_item = &g_global_vars->crafting_grid_items[y][x];
    //        std::string item_id = grid_item->count ? grid_item->id : "";

    //        if (real_x < 0 || real_y < 0 || real_y >= recipe.size() || real_x >= recipe.at(real_y).size())
    //            continue;

    //        if (item_id == "")
    //            continue;

    //        if (recipe[real_y][real_x] == item_id)
    //            found_matches++;
    //    }
    //}

    // is it perfect matching or no?
    return found_matches == needed_matches;
}

bool c_crafting_recipe::can_be_crafted()
{
    /*e_grid_type grid = get_grid_type();
    e_grid_type inventory_grid = g_global_vars->crafting_grid;

    bool matching_type = inventory_grid >= grid;
    if (!matching_type)
        return false;

    glm::vec2 size = get_size();

    int grid_x = inventory_grid == e_grid_type::GRID_2X2 ? 2 : 3;
    int grid_y = inventory_grid == e_grid_type::GRID_2X2 ? 2 : 3;

    for (int x = 0; x < grid_x; x++)
    {
        for (int y = 0; y < grid_y; y++)
        {
            if (check_crafting_recipe(inventory_grid, size, glm::vec2(x, y)))
                return true;
        }
    }

    return false;*/
}

void c_crafting_recipe::process_crafting()
{
    
}

void c_crafting_recipe::on_take_item()
{
    
}

c_item* c_inventory_item::get_ptr()
{
    return is_empty() ? g_global_vars->mapped_items[id] : nullptr;
}