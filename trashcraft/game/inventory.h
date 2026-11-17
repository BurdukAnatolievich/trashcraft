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
	c_inventory_item items[INVENTORY_BAR_SIZE * (INVENTORY_BAR_COUNT + 1)];

	c_inventory_item crafting_product;
	c_inventory_item crafting_grid_items[4];

	int current_hotbar_slot = 0;

	inline void update_hotbar(int next_slot) { current_hotbar_slot += next_slot; if (current_hotbar_slot > (INVENTORY_BAR_SIZE - 1)) current_hotbar_slot = 0; if (current_hotbar_slot < 0) current_hotbar_slot = INVENTORY_BAR_SIZE - 1; };

	inline c_inventory_item* get_item(uint16_t bar, uint16_t slot) { return &items[bar * INVENTORY_BAR_SIZE + slot]; };
	inline c_inventory_item* get_crafting_item(uint16_t bar, uint16_t slot) { return &crafting_grid_items[bar * 2 + slot]; };

	inline c_inventory_item* get_hotbar_item(int slot) { return &items[BAR_HOTBAR * INVENTORY_BAR_SIZE + slot]; };
	inline c_inventory_item* get_hotbar_item() { return &items[BAR_HOTBAR * INVENTORY_BAR_SIZE + current_hotbar_slot]; };

	inline c_inventory_item* get_inventory() { return items; };
	inline c_inventory_item* get_hotbar() { return items + (INVENTORY_BAR_SIZE * INVENTORY_BAR_COUNT); };
};