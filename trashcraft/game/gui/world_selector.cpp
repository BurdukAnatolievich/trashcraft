#include "world_selector.h"

#include <menu.h>
#include <game.h>
#include <save.h>

void c_world_selector::refresh_world_list()
{
    
}

void c_world_selector::load_selected_world()
{
    std::deque<c_world_save::world_info_t>& worlds = g_world_save->worlds;
    if (selected_index < 0 || selected_index >= worlds.size()) {
        return;
    }

    g_game->start();
    g_ui_manager->clear_screens();
}

void c_world_selector::delete_selected_world()
{
    std::deque<c_world_save::world_info_t>& worlds = g_world_save->worlds;
    if (selected_index < 0 || selected_index >= worlds.size()) {
        return;
    }

    worlds.erase(worlds.begin() + selected_index);
    selected_index = -1;
}

void c_world_selector::update()
{

}

bool c_world_selector::on_mouse_click(int button, float x, float y)
{
    if (button != 0) {
        return false;
    }

    std::deque<c_world_save::world_info_t>& worlds = g_world_save->worlds;

    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
    float cx = display.width * 0.5f;

    float list_top = 32.0f * last_scale;
    float list_bottom = display.height - 64.0f * last_scale;

    float slot_w = 220.0f * last_scale;
    float slot_h = 36.0f * last_scale;

    float slot_x = cx - slot_w * 0.5f;
    if (x >= slot_x && x <= slot_x + slot_w && y >= list_top && y <= list_bottom) {
        float relative_y = (y - (list_top + 4.0f * last_scale)) + scroll_offset;
        if (relative_y >= 0.0f) {
            int clicked_idx = relative_y / slot_h;
            if (clicked_idx >= 0 && clicked_idx < worlds.size()) {
                selected_index = clicked_idx;
                return true;
            }
        }
    }

    return false;
}

bool c_world_selector::on_mouse_scroll(int direction)
{
    std::deque<c_world_save::world_info_t>& worlds = g_world_save->worlds;

    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
    float view_h = (display.height - 64.0f * last_scale) - (32.0f * last_scale);
    float content_h = static_cast<float>(worlds.size()) * (36.0f * last_scale) + 8.0f * last_scale;
    float max_scroll = std::max(0.0f, content_h - view_h);

    if (max_scroll <= 0.0f) {
        return false;
    }

    float scroll_step = 18.0f * last_scale;
    scroll_offset = std::clamp(scroll_offset - static_cast<float>(direction) * scroll_step, 0.0f, max_scroll);
    return true;
}

void c_world_selector::render(float scale)
{
    last_scale = scale;

    std::deque<c_world_save::world_info_t>& worlds = g_world_save->worlds;

    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
    float cx = std::floor(display.width * 0.5f);

    float list_top = std::floor(32.0f * scale);
    float list_bottom = std::floor(display.height - 64.0f * scale);
    float list_h = list_bottom - list_top;

    g_menu_draw->dirt_background(0.0f, list_top, display.width, list_h, scale, scroll_offset, IM_COL32(32, 32, 32, 255));

    float slot_w = std::floor(220.0f * scale);
    float slot_h = std::floor(36.0f * scale);
    float slot_x = cx - std::floor(slot_w * 0.5f);

    for (size_t i = 0; i < worlds.size(); ++i) {
        float item_y = list_top + std::floor(4.0f * scale) + (static_cast<float>(i) * slot_h) - scroll_offset;
        if (item_y + slot_h < list_top || item_y > list_bottom) {
            continue;
        }

        const auto& world = worlds[i];
        if (i == selected_index) {
            g_menu_draw->rect_filled(slot_x - 2.0f * scale, item_y - 2.0f * scale, slot_x + slot_w + 2.0f * scale, item_y + slot_h - 2.0f * scale, IM_COL32(128, 128, 128, 255));
            g_menu_draw->rect_filled(slot_x - 1.0f * scale, item_y - 1.0f * scale, slot_x + slot_w + 1.0f * scale, item_y + slot_h - 3.0f * scale, IM_COL32(0, 0, 0, 255));
        }

        float text_x = slot_x + 2.0f * scale;
        g_menu_draw->text(world.display_name, text_x, item_y + 1.0f * scale, scale);
        g_menu_draw->text(world.actual_name + " ( Kalovoziki 2026 )", text_x, item_y + 12.0f * scale, scale);

        std::string line3 = "Survival, Cheats";
        g_menu_draw->text(line3, text_x, item_y + 22.0f * scale, scale);
    }

    g_menu_draw->dirt_background(0.0f, 0.0f, display.width, list_top, scale, 0.0f, IM_COL32(64, 64, 64, 255));
    g_menu_draw->dirt_background(0.0f, list_bottom, display.width, 64.0f * scale, scale, 0.0f, IM_COL32(64, 64, 64, 255));

    g_menu_draw->text("lang_select_world_title", cx - 40.0f * scale, 20.0f * scale, scale);

    float row1_y = std::floor(display.height - 52.0f * scale);
    float row2_y = std::floor(display.height - 28.0f * scale);
    float btn_h = std::floor(20.0f * scale);

    float left_edge = cx - std::floor(154.0f * scale);
    float gap = std::floor(4.0f * scale);

    float top_btn_w = std::floor(152.0f * scale);
    float sub_btn_w = std::floor(74.0f * scale);

    float step_x = sub_btn_w + gap;

    bool has_selection = (selected_index >= 0 && selected_index < static_cast<int>(worlds.size()));

    if (g_menu_draw->button("lang_select_world_play", left_edge, row1_y, top_btn_w, btn_h, scale) && has_selection) {
        load_selected_world();
        return;
    }

    if (g_menu_draw->button("lang_select_world_create", left_edge + top_btn_w + gap, row1_y, top_btn_w, btn_h, scale)) {

    }

    if (g_menu_draw->button("lang_select_world_rename", left_edge, row2_y, sub_btn_w, btn_h, scale) && has_selection) {

    }

    if (g_menu_draw->button("lang_select_world_delete", left_edge + step_x, row2_y, sub_btn_w, btn_h, scale) && has_selection) {
        delete_selected_world();
    }

    if (g_menu_draw->button("lang_select_world_recreate", left_edge + step_x * 2.0f, row2_y, sub_btn_w, btn_h, scale) && has_selection) {

    }

    if (g_menu_draw->button("lang_select_world_cancel", left_edge + step_x * 3.0f, row2_y, sub_btn_w, btn_h, scale)) {
        g_ui_manager->pop_screen();
        return;
    }
}