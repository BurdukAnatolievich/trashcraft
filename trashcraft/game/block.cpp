#include "block.h"
#include <raytrace/models.h>

c_block::c_block(std::string register_name, std::string register_id, std::vector<std::string> material_array, glm::vec3 mins, glm::vec3 maxs, float hardness, int mining_tool, int mining_tool_to_drop, int mining_tool_class, std::string drop_item, bool is_solid, bool is_transparent, bool is_light, bool is_liquid, int block_type, std::string hotbar_texture, int interactable, std::string model)
{
    this->type = c_item::e_type::BLOCK;
    this->block_type = block_type;

    this->name = register_name;
    this->id = register_id;

    this->numeric_id = g_global_vars->items.size();

    for (int i = c_block::e_material::SIDE; i < c_block::e_material::MAX; i++)
        this->materials[i] = g_texture_manager->find_texture_by_name(material_array[i]);

    this->item_material = g_texture_manager->find_texture_by_name(hotbar_texture);
    this->item_model = g_model_manager->find_model_by_name(hotbar_texture);

    this->interactable = interactable;

    this->is_solid = is_solid;
    this->is_transparent = is_transparent;
    this->is_light = is_light;
    this->is_liquid = is_liquid;

    this->hardness = hardness;

    this->mining_tool = mining_tool;
    this->mining_tool_to_drop = mining_tool_to_drop;
    this->mining_tool_class = mining_tool_class;

    this->drop_item = drop_item != "" ? g_global_vars->items[drop_item] : nullptr;

    this->min = mins;
    this->max = maxs;

    g_global_vars->items[register_id] = this;
}

c_scene_object* c_block::create_scene_object(const glm::vec3& origin, float scale)
{
    c_box* scene_box = new c_box(id, origin, min, max, materials[c_block::e_material::SIDE], materials[c_block::e_material::UPPER], materials[c_block::e_material::BOTTOM], is_transparent);

    scene_box->is_light = is_light;
    scene_box->light_intensity = 0.5f;

    scene_box->scale = glm::vec3(scale);

    return scene_box;
}
