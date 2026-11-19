#include "hud.h"
#include "menu.h"
#include "global_vars.h"
#include "entity/entity_list.h"

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

    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
    float cx = display.width * 0.5f;
    float cy = display.height * 0.5f;

    g_menu_draw->sprite(mc_sprites::CROSSHAIR, cx - 8.0f * scale, cy - 8.0f * scale, scale);

    float hotbar_w = 182.0f * scale;
    float hotbar_h = 22.0f * scale;

    float hotbar_x = cx - hotbar_w * 0.5f;
    float hotbar_y = display.height - hotbar_h;

    float status_y = hotbar_y - 10.0f * scale;

    g_menu_draw->sprite(mc_sprites::HOTBAR, hotbar_x, hotbar_y, scale);

    int slot = local->inventory.current_hotbar_slot;
    g_menu_draw->sprite(mc_sprites::HOTBAR_SELECTOR, hotbar_x - 1.0f * scale + (slot * 20.0f * scale), hotbar_y - 1.0f * scale, scale);

    for (int i = 0; i < INVENTORY_BAR_SIZE; i++) {
		g_menu_draw->draw_items(local->inventory.get_hotbar_item(i), hotbar_x + 3.0f * scale + i * 20.0f * scale, hotbar_y + 3.0f * scale, scale);
    }

    if (local->game_mode != e_game_mode::CREATIVE) {
        int total_hearts = static_cast<int>(std::ceil(local->max_health * 0.5f));
        int current_hp = static_cast<int>(std::ceil(local->health));

        for (int i = 0; i < total_hearts; i++) {
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
}