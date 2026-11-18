#pragma once
#include "gui_screen.h"

class c_world_selector : public c_gui_screen
{
    float last_scale = 0.0f;
	float scroll_offset = 0.0f;

    void refresh_world_list();
    void load_selected_world();
    void load_create_world();
    void delete_selected_world();
public:
    c_world_selector() {

    }

    void update() override;
    void render(float scale) override;

    bool on_mouse_click(int button, float x, float y) override;
    bool on_mouse_scroll(int direction) override;

    bool is_full_screen() const override { return true; }
};