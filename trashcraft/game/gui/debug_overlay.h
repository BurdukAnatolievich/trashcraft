#pragma once
#include "overlay.h"

class c_debug_overlay : public c_gui_overlay
{
public:
    c_debug_overlay() {

    }

    void update() override;
    void render(float scale) override;
};