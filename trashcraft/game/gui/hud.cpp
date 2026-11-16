#include "hud.h"

#include <menu.h>
#include <global_vars.h>

#include <algorithm>

void c_hud_overlay::update()
{

}

void c_hud_overlay::render(float scale)
{
    ImVec2 display = ImGui::GetIO().DisplaySize;
    float cx = display.x * 0.5f;
    float cy = display.y * 0.5f;

    float health = 20.0f;
    float max_health = 20.0f;

    int armor = 12;
    int food = 18;

    float xp_progress = 0.65f;

    int xp_level = 14;

    g_menu_draw->sprite(
        mc_sprites::CROSSHAIR,
        cx - 8.0f * scale,
        cy - 8.0f * scale,
        scale
    );

    // Хотбар и селектор
    float hotbar_w = 182.0f * scale;
    float hotbar_h = 22.0f * scale;
    float hotbar_x = cx - hotbar_w * 0.5f;
    float hotbar_y = display.y - hotbar_h;

    g_menu_draw->sprite(mc_sprites::HOTBAR, hotbar_x, hotbar_y, scale);

    int slot = std::clamp(g_global_vars->current_inventory_slot, 0, 8);
    g_menu_draw->sprite(
        mc_sprites::HOTBAR_SELECTOR,
        hotbar_x - 1.0f * scale + (slot * 20.0f * scale),
        hotbar_y - 1.0f * scale,
        scale
    );

    // Полоска XP
    float xp_y = hotbar_y - 7.0f * scale;
    g_menu_draw->sprite(mc_sprites::XP_BAR_BG, hotbar_x, xp_y, scale);

    float clamped_xp = std::clamp(xp_progress, 0.0f, 1.0f);
    if (clamped_xp > 0.0f) {
        g_menu_draw->sprite("icons", hotbar_x, xp_y, 0.0f, 69.0f, 182.0f * clamped_xp, 5.0f, scale);
    }

    // Уровень XP
    if (xp_level > 0) {
        std::string lvl_str = std::to_string(xp_level);
        float text_w = ImGui::CalcTextSize(lvl_str.c_str()).x * scale;
        float text_x = cx - text_w * 0.5f;
        float text_y = xp_y - 8.0f * scale;

        g_menu_draw->text(lvl_str, text_x + 1.0f * scale, text_y + 1.0f * scale, scale, IM_COL32(0, 0, 0, 255));
        g_menu_draw->text(lvl_str, text_x, text_y, scale, IM_COL32(128, 255, 32, 255));
    }

    float status_y = xp_y - 10.0f * scale;

    // Здоровье
    int total_hearts = static_cast<int>(std::ceil(max_health * 0.5f));
    int current_hp = static_cast<int>(std::ceil(health));

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

    // Броня
    if (armor > 0) {
        float armor_y = status_y - 10.0f * scale;
        for (int i = 0; i < 10; ++i) {
            float ax = hotbar_x + i * 8.0f * scale;
            g_menu_draw->sprite(mc_sprites::ARMOR_BG, ax, armor_y, scale);

            int armor_val = armor - (i * 2);
            if (armor_val >= 2) {
                g_menu_draw->sprite(mc_sprites::ARMOR_FULL, ax, armor_y, scale);
            }
            else if (armor_val == 1) {
                g_menu_draw->sprite(mc_sprites::ARMOR_HALF, ax, armor_y, scale);
            }
        }
    }

    // Голод
    float food_start_x = hotbar_x + hotbar_w - 9.0f * scale;
    for (int i = 0; i < 10; ++i) {
        float fx = food_start_x - i * 8.0f * scale;
        g_menu_draw->sprite(mc_sprites::FOOD_BG, fx, status_y, scale);

        int food_val = food - (i * 2);
        if (food_val >= 2) {
            g_menu_draw->sprite(mc_sprites::FOOD_FULL, fx, status_y, scale);
        }
        else if (food_val == 1) {
            g_menu_draw->sprite(mc_sprites::FOOD_HALF, fx, status_y, scale);
        }
    }
}