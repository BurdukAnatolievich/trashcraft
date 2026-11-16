#pragma once
#include "overlay.h"

class c_hud_overlay : public c_gui_overlay
{
public:
    c_hud_overlay() {

    }

    void update() override;
    void render(float scale) override;
};