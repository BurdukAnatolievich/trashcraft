#pragma once
#include "gui_screen.h"

class c_gui_overlay : public c_gui_screen
{
public:
    c_gui_overlay() {

    }

    bool can_close_with_esc() const override { return false; };
};