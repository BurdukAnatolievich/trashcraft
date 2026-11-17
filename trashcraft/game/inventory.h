#pragma once
#include "item.h"
#include "config.h"

enum e_inventory_bar
{
    BAR_INVALID = -1,
    BAR_HOTBAR = 3,
    BAR_MAX
};

class c_inventory
{
public:
	c_inventory_item items[9 * (INVENTORY_BAR_COUNT + 1)];

	c_inventory_item crafting_product;
	c_inventory_item crafting_grid_items[4];

	inline c_inventory_item* get_inventory() { return items; };
	inline c_inventory_item* get_hotbar() { return items + (9 * INVENTORY_BAR_COUNT); };
};