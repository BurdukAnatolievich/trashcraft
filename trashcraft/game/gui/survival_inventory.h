#pragma once
#include "overlay.h"
#include <inventory.h>

class c_survival_inventory : public c_gui_screen
{
    float last_scale = 0.0f;
    c_inventory_item cursor_item;

    int build_slots(c_inventory* inventory, slot_entry_t* out_slots);
    void handle_slot_click(c_inventory* inventory, slot_entry_t& slot, bool right_click);
    void render_slot_item(c_inventory_item* item, float x, float y, float scale);
public:
    c_survival_inventory() = default;

    void update() override;
    void render(float scale) override;

    bool on_mouse_click(int button, float x, float y) override;
    bool on_key_press(int key) override;
};