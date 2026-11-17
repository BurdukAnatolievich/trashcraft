#pragma once
#include "overlay.h"

class c_inventory_overlay : public c_gui_overlay
{
public:
    c_inventory_overlay() {

    }

    void update() override;
    void render(float scale) override;
};