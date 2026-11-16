#include "input.h"
#include "global_vars.h"
#include "block.h"
#include "world.h"
#include "menu.h"
#include "event_manager.h"

#include <rabbit3d.h>

#include <Windows.h>

c_input* g_input = new c_input;

void c_input::initialize()
{
    key_bindings[BIND_MOVE_FORWARD] = { ImGuiKey_W, "lang_input_move_forward", 0 };
    key_bindings[BIND_MOVE_BACKWARD] = { ImGuiKey_S, "lang_input_move_backward", 0 };
    key_bindings[BIND_MOVE_RIGHT] = { ImGuiKey_D, "lang_input_move_right", 0 };
    key_bindings[BIND_MOVE_LEFT] = { ImGuiKey_A, "lang_input_move_left", 0 };
    key_bindings[BIND_JUMP] = { ImGuiKey_Space, "lang_input_move_jump", 0 };
    key_bindings[BIND_SPRINT] = { ImGuiKey_LeftCtrl, "lang_input_move_sprint", 0, };

    key_bindings[BIND_ATTACK] = { ImGuiKey_MouseLeft, "lang_input_attack", 0 };
    key_bindings[BIND_USE] = { ImGuiKey_MouseRight, "lang_input_use", 0 };
}

void c_input::update()
{
    handle_player_input();

    for (int i = 0; i < BIND_MAX; i++) {
        key_bind_t& bind = key_bindings[i];
        if (!bind.key) {
            continue;
        }

        if (is_key_down(bind.key)) {
            bind.last_press_tick = g_time->get_tick_count();
        }
    }
}

void c_input::handle_player_input()
{
    c_player* player = g_global_vars->local_player;
    if (!player)
        return;

    c_camera* camera = player->get_bound_camera();
    if (!camera)
        return;

    int new_buttons = 0;

    if (!is_locked)
    {
        float pitch_change = mouse_delta.y * 0.25f;
        float yaw_change = mouse_delta.x * 0.25f;

        camera->process_input({ pitch_change, yaw_change });

        if (is_bind_down(BIND_MOVE_FORWARD))
            new_buttons |= e_buttons::IN_MOVE_FORWARD;

        if (is_bind_down(BIND_MOVE_BACKWARD))
            new_buttons |= e_buttons::IN_MOVE_BACKWARD;

        if (is_bind_down(BIND_MOVE_RIGHT))
            new_buttons |= e_buttons::IN_MOVE_RIGHT;

        if (is_bind_down(BIND_MOVE_LEFT))
            new_buttons |= e_buttons::IN_MOVE_LEFT;

        if (is_bind_down(BIND_JUMP))
            new_buttons |= e_buttons::IN_JUMP;

        if (is_bind_down(BIND_ATTACK))
            new_buttons |= e_buttons::IN_ATTACK;

        if (is_bind_down(BIND_USE))
            new_buttons |= e_buttons::IN_ACTION;

        if (is_bind_down(BIND_SPRINT) || is_bind_double_pressed(BIND_MOVE_FORWARD))
            new_buttons |= e_buttons::IN_SPRINT;

        {
            //std::lock_guard<std::mutex> input_guard(input_swap_mutex);
            accumulated_input.angle_change.x += pitch_change;
            accumulated_input.angle_change.y += yaw_change;

            accumulated_input.buttons = new_buttons;
        }

        if (mouse_scroll < 0.0f)
            g_global_vars->current_inventory_slot++;
        if (mouse_scroll > 0.0f)
            g_global_vars->current_inventory_slot--;

        mouse_scroll = 0.0f;

        g_global_vars->current_inventory_slot %= 9;
    }
}

bool c_input::is_bind_pressed(uint8_t bind)
{
    uint16_t key = key_bindings[bind].key;
    if (!key) {
        return false;
    }

    return ImGui::IsKeyPressed((ImGuiKey)key, false);
}

bool c_input::is_bind_double_pressed(uint8_t bind)
{
    uint16_t key = key_bindings[bind].key;
    if (!key) {
        return false;
    }

    uint64_t ticks_passed = g_time->get_tick_count() - key_bindings[bind].last_press_tick;
    return ticks_passed <= 7 && ImGui::IsKeyPressed((ImGuiKey)key, false);
}

bool c_input::is_bind_down(uint8_t bind)
{
    uint16_t key = key_bindings[bind].key;
    if (!key) {
        return false;
    }

    return ImGui::IsKeyDown((ImGuiKey)key);
}

bool c_input::is_bind_released(uint8_t bind)
{
    uint16_t key = key_bindings[bind].key;
    if (!key) {
        return false;
    }

    return ImGui::IsKeyReleased((ImGuiKey)key);
}

bool c_input::is_key_pressed(uint16_t key)
{
    return ImGui::IsKeyPressed((ImGuiKey)key, false);
}

bool c_input::is_key_down(uint16_t key)
{
    return ImGui::IsKeyDown((ImGuiKey)key);
}

bool c_input::is_key_released(uint16_t key)
{
    return ImGui::IsKeyReleased((ImGuiKey)key);
}