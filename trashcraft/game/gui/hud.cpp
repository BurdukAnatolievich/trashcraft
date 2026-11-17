#include "hud.h"

#include <menu.h>
#include <global_vars.h>
#include <entity/entity_list.h>

#include <algorithm>

void c_hud_overlay::update()
{

}

void c_hud_overlay::render(float scale)
{
    c_player* local = g_global_vars->local_player;
    if (!local) {
        return;
    }

    ImVec2 display = ImGui::GetIO().DisplaySize;
    float cx = display.x * 0.5f;
    float cy = display.y * 0.5f;

    g_menu_draw->sprite(mc_sprites::CROSSHAIR, cx - 8.0f * scale, cy - 8.0f * scale, scale);

    float hotbar_w = 182.0f * scale;
    float hotbar_h = 22.0f * scale;

    float hotbar_x = cx - hotbar_w * 0.5f;
    float hotbar_y = display.y - hotbar_h;

    float status_y = hotbar_y - 10.0f * scale;

    g_menu_draw->sprite(mc_sprites::HOTBAR, hotbar_x, hotbar_y, scale);

    int slot = std::clamp(g_global_vars->current_inventory_slot, 0, 8);
    g_menu_draw->sprite(mc_sprites::HOTBAR_SELECTOR, hotbar_x - 1.0f * scale + (slot * 20.0f * scale), hotbar_y - 1.0f * scale, scale);

    int total_hearts = static_cast<int>(std::ceil(local->max_health * 0.5f));
    int current_hp = static_cast<int>(std::ceil(local->health));

    for (int i = 0; i < total_hearts; ++i) {
        float hx = hotbar_x + i * 8.0f * scale;
        g_menu_draw->sprite(mc_sprites::HEART_BG, hx, status_y, scale);

        int heart_hp = current_hp - (i * 2);
        if (heart_hp >= 2) {
            g_menu_draw->sprite(mc_sprites::HEART_FULL, hx, status_y, scale);
        }
        else if (heart_hp == 1) {
            g_menu_draw->sprite(mc_sprites::HEART_HALF, hx, status_y, scale);
        }
    }
}