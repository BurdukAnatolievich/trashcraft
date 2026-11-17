#include "main_menu.h"

#include <menu.h>
#include <game.h>

void c_main_menu::update() 
{

}

void c_main_menu::render(float scale) 
{
    ImVec2 display = ImGui::GetIO().DisplaySize;
    float cx = display.x * 0.5f;

    float total_logo_w = 274.0f * scale;
    float logo_x = cx - total_logo_w * 0.5f;
    float logo_y = 30.0f * scale;

    g_menu_draw->sprite(mc_sprites::TITLE_MINE, logo_x, logo_y, scale);
    g_menu_draw->sprite(mc_sprites::TITLE_CRAFT, logo_x + 155.0f * scale, logo_y, scale);

    float btn_w = 200.0f * scale;
    float btn_h = 20.0f * scale;
    float btn_x = cx - btn_w * 0.5f;
    float start_y = display.y * 0.25f + 48.0f * scale;

    if (g_menu_draw->button("lang_main_menu_singleplayer", btn_x, start_y, btn_w, btn_h, scale)) {
        g_game->start();
        g_ui_manager->pop_screen();
    }

    if (g_menu_draw->button("lang_main_menu_mods", btn_x, start_y + 24.0f * scale, btn_w, btn_h, scale)) {

    }

    float half_w = 98.0f * scale;
    float bottom_btn_y = start_y + 72.0f * scale;

    if (g_menu_draw->button("lang_main_menu_about", btn_x, bottom_btn_y, half_w, btn_h, scale)) {

    }

    if (g_menu_draw->button("lang_main_menu_quit", btn_x + 102.0f * scale, bottom_btn_y, half_w, btn_h, scale)) {

    }

    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float footer_y = display.y - 12.0f * scale;

	g_menu_draw->text("Trashcraft Alpha v" + std::string(TRASHCRAFT_VERSION_STRING), 10.0f, footer_y, scale);
}