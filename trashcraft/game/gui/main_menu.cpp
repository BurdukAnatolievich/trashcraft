#include "main_menu.h"
#include "world_selector.h"

#include <menu.h>
#include <game.h>

void c_main_menu::update() 
{

}

void c_main_menu::render(float scale) 
{
    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();

    float cx = display.width * 0.5f;
    float total_logo_w = 274.0f * scale;

    float logo_x = cx - total_logo_w * 0.5f;
    float logo_y = 30.0f * scale;

    g_menu_draw->sprite(mc_sprites::TITLE_MINECRAFT, logo_x, logo_y, scale);

    float btn_w = 200.0f * scale;
    float btn_h = 20.0f * scale;

    float btn_x = cx - btn_w * 0.5f;
    float btn_y = display.height * 0.25f + 48.0f * scale;

    if (g_menu_draw->button("lang_main_menu_singleplayer", btn_x, btn_y, btn_w, btn_h, scale)) {
		g_ui_manager->push_screen(new c_world_selector());
    }

    if (g_menu_draw->button("lang_main_menu_mods", btn_x, btn_y + 24.0f * scale, btn_w, btn_h, scale)) {

    }

    float half_w = 98.0f * scale;
    float bottom_btn_y = btn_y + 72.0f * scale;

    if (g_menu_draw->button("lang_main_menu_about", btn_x, bottom_btn_y, half_w, btn_h, scale)) {

    }

    if (g_menu_draw->button("lang_main_menu_quit", btn_x + 102.0f * scale, bottom_btn_y, half_w, btn_h, scale)) {

    }

    float footer_y = display.height - 12.0f * scale;
	g_menu_draw->text("Trashcraft Alpha v" + std::string(TRASHCRAFT_VERSION_STRING), 10.0f, footer_y, scale);
}