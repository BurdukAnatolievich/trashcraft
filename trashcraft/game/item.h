#pragma once
#include <string>
#include <vector>

#include <glm.hpp>

class c_texture;
class c_model;

class c_registry
{
public:
    std::string id = "none";
    std::string name = "none";

    std::string description = "";

    uint16_t numeric_id = 0;

    inline std::string get_name() { return name; };
    inline std::string get_id() { return id; };

    inline uint16_t get_numeric_id() { return numeric_id; };
};

class c_item : public c_registry
{
public:
    enum e_type
    {
        ITEM,
        BLOCK,
        TOOL,
        RECORD
    };

    c_item();
    c_item(std::string register_name, std::string register_id, int max_count = 64);
    c_item(std::string register_name, std::string register_id, std::string material, int max_count = 64);

    uint16_t type = 0;
    uint16_t max_count = 64;

    c_model* item_model = nullptr;
    c_texture* item_material = nullptr;
};

class c_tool : public c_item
{
public:
    enum e_tool : uint16_t
    {
        NONE = (1 << 0),
        ANY = (1 << 1),
        PICKAXE = (1 << 2),
        SHOVEL = (1 << 3),
        AXE = (1 << 4),
        HOE = (1 << 5),
    };

    c_tool();
    c_tool(std::string register_name, std::string register_id, int tool_type, int tool_durability, int tool_efficiency);
    c_tool(std::string register_name, std::string register_id, int tool_type, int tool_durability, int tool_efficiency, std::string material);

    uint16_t tool = 0;

    uint16_t durability = 0;
    uint16_t efficiency = 0;
};

class c_music_disc : public c_item
{
public:
    c_music_disc();
    c_music_disc(std::string register_name, std::string register_id, std::string register_desc, std::string music_id);
    c_music_disc(std::string register_name, std::string register_id, std::string register_desc, std::string music_id, std::string material);

    char music_id[64] = { 0 };
};

class c_inventory_item
{
public:
    uint16_t id = 0;

    uint16_t count = 0;
    uint16_t usage_count = 0;

	inline bool is_empty() { return !count || !id; };
    inline void decrement_count(uint16_t amount = 1) { count = std::max(0, count - amount); if (!count) reset(); };

    inline void reset() { id = count = usage_count = 0; };
    c_item* get_ptr();
};

class c_crafting_recipe : public c_registry
{
public:
    enum e_grid_type
    {
        NONE = -1,
        GRID_2X2,
        GRID_3X3
    };

    c_crafting_recipe();
    c_crafting_recipe(std::string register_name, std::string register_id, const std::vector<std::vector<std::string>>& recipe, std::string product_item_id, int product_item_count = 1);

    std::vector<std::vector<std::string>> recipe;

    std::string product_item_id = "";
    int product_item_count = 1;

    glm::vec2 get_size();
    e_grid_type get_grid_type();

    bool check_crafting_recipe(c_crafting_recipe::e_grid_type grid, glm::vec2 size, glm::vec2 position);
    bool can_be_crafted();

    void process_crafting();
    void on_take_item();
};
