#pragma once
#include "gui_screen.h"

class c_main_menu : public c_gui_screen
{
public:
    c_main_menu() {

    }

    void update() override;
    void render(float scale) override;

    bool is_full_screen() const override { return true; };
};