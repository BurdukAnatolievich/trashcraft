#include "menu.h"
#include "input.h"
#include "game.h"

#include <rabbit3d.h>

#include <imgui_internal.h>
#include <imgui.h>

c_ui_manager* g_ui_manager = new c_ui_manager;

void c_ui_manager::on_escape_pressed() 
{
    c_gui_screen* active = get_active_screen();
    if (active) {
        if (active->can_close_with_esc()) {
            pop_screen();
        }
    }
    else {
        //push_screen(new c_pause_screen());
    }
}

void c_ui_manager::render() 
{
    ImVec2 display = ImGui::GetIO().DisplaySize;
    if (display.x <= 0 || display.y <= 0)
        return;

    float scale = 2.0f;

    c_gui_screen* active = get_active_screen();

    bool in_game_hud = (!active || !active->is_full_screen());
    if (in_game_hud && !hide_hud) {
        hud.render(scale);
        if (show_debug_f3) {
            debug_overlay.render(scale);
        }
    }

    if (active) {
        constexpr ImGuiKey mouse_keys[] = { ImGuiKey_MouseLeft, ImGuiKey_MouseRight, ImGuiKey_MouseMiddle };
        for (ImGuiKey btn : mouse_keys) {
            if (g_input->is_key_pressed(btn)) { 
				glm::vec2 mouse_pos = g_input->get_mouse_position();
                active->on_mouse_click(btn, mouse_pos.x, mouse_pos.y);
            }
        }

        if (!active->is_full_screen()) {
            ImGui::GetBackgroundDrawList()->AddRectFilled(
                ImVec2(0, 0), display, IM_COL32(0, 0, 0, 140)
            );
        }
        active->render(scale);
    }

    process_pending_actions();
}

void c_ui_manager::process_pending_actions() 
{
    if (pending_clear) {
        for (c_gui_screen* screen : screen_stack) {
            delete screen;
        }
        screen_stack.clear();
        pending_clear = false;
        pending_pop = false;
        update_cursor_state();
    }
    else if (pending_pop) {
        if (!screen_stack.empty()) {
            delete screen_stack.back();
            screen_stack.pop_back();
        }
        pending_pop = false;
        update_cursor_state();
    }
}

void c_ui_manager::update_cursor_state() 
{
    bool unlock_mouse = has_open_screen();
    // g_window->set_cursor_locked(!unlock_mouse);
}