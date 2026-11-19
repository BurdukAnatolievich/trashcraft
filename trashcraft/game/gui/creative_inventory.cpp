#include "creative_inventory.h"
#include "menu.h"
#include "global_vars.h"
#include "entity/entity_list.h"

#include <algorithm>

void c_creative_inventory::render_slot_item(c_inventory_item* item, float x, float y, float scale)
{
    if (!item || item->is_empty())
        return;

    g_menu_draw->draw_items(item, x, y, scale);
}

int c_creative_inventory::build_slots(c_inventory* inventory, slot_entry_t* out_slots)
{
    int count = 0;
    for (int col = 0; col < INVENTORY_BAR_SIZE; col++) {
        out_slots[count++] = { inventory->get_hotbar_item(col), 8.0f + col * 18.0f, 184.0f, false };
    }
    return count;
}

void c_creative_inventory::handle_slot_click(c_inventory* inventory, slot_entry_t& slot, bool right_click, bool shift_held)
{
    c_inventory_item* target = slot.item;
    if (!target)
        return;

    if (shift_held) {
        target->reset();
        return;
    }

    if (right_click) {
        if (!cursor_item.is_empty()) {
            if (target->is_empty()) {
                *target = cursor_item;
                target->count = 1;
            }
            else if (target->id == cursor_item.id && target->count < 64) {
                target->count++;
            }
        }
        else if (!target->is_empty()) {
            int half = (target->count + 1) / 2;
            cursor_item = *target;
            cursor_item.count = half;
            target->count -= half;
            if (target->count <= 0)
                target->reset();
        }
    }
    else {
        if (!cursor_item.is_empty()) {
            *target = cursor_item;
        }
        else if (!target->is_empty()) {
            cursor_item = *target;
            target->reset();
        }
    }
}

void c_creative_inventory::update()
{
    int valid_items = 0;
    for (const auto& pair : g_global_vars->items) {
        if (pair.second && pair.second->numeric_id != 0)
            ++valid_items;
    }

    int total_rows = std::max(0, (valid_items + 7) / 8 - 8);

    if (total_rows <= 0) {
        scroll_progress = 0.0f;
        is_scrolling = false;
        return;
    }

    if (is_scrolling) {
        if (!g_input->is_key_down(ImGuiKey_MouseLeft)) {
            is_scrolling = false;
            return;
        }

        c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
        float inv_y = std::floor((display.height - 208.0f * last_scale) * 0.5f);
        float track_top = inv_y + 18.0f * last_scale;
        float track_h = 144.0f * last_scale;
        float thumb_h = 15.0f * last_scale;

        glm::vec2 mouse_pos = g_input->get_mouse_position();
        float rel_y = mouse_pos.y - track_top - thumb_h * 0.5f;
        scroll_progress = std::clamp(rel_y / (track_h - thumb_h), 0.0f, 1.0f);
    }
}

bool c_creative_inventory::on_mouse_click(int button, float x, float y)
{
    if (button != ImGuiKey_MouseLeft && button != ImGuiKey_MouseRight)
        return false;

    c_player* local = g_global_vars->local_player;
    if (!local)
        return false;

    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
    float inv_w = 176.0f * last_scale;
    float inv_h = 208.0f * last_scale;
    float inv_x = std::floor((display.width - inv_w) * 0.5f);
    float inv_y = std::floor((display.height - inv_h) * 0.5f);
    float slot_size = 16.0f * last_scale;

    int valid_items = 0;
    for (const auto& pair : g_global_vars->items) {
        if (pair.second && pair.second->numeric_id != 0)
            ++valid_items;
    }
    int total_rows = std::max(0, (valid_items + 7) / 8 - 8);

    float sb_x = std::floor(inv_x + 156.0f * last_scale);
    float sb_y = std::floor(inv_y + 18.0f * last_scale);
    float sb_h = 142.0f * last_scale;
    if (button == ImGuiKey_MouseLeft &&
        x >= sb_x && x < sb_x + 12.0f * last_scale &&
        y >= sb_y && y < sb_y + sb_h) {
        if (total_rows > 0) {
            is_scrolling = true;
            float thumb_h = 15.0f * last_scale;
            float rel_y = y - sb_y - thumb_h * 0.5f;
            scroll_progress = std::clamp(rel_y / (sb_h - thumb_h), 0.0f, 1.0f);
        }
        return true;
    }

    bool shift_held = g_input->is_key_down(ImGuiKey_LeftShift) || g_input->is_key_down(ImGuiKey_RightShift);

    int skip = static_cast<int>(scroll_progress * total_rows + 0.5f) * 8;
    auto it = g_global_vars->items.begin();
    int skipped = 0;
    while (it != g_global_vars->items.end() && skipped < skip) {
        if (it->second && it->second->numeric_id != 0)
            ++skipped;
        ++it;
    }

    for (int i = 0; i < 64; ++i) {
        while (it != g_global_vars->items.end() && (!it->second || it->second->numeric_id == 0))
            ++it;

        float sx = std::floor(inv_x + (8.0f + (i % 8) * 18.0f) * last_scale);
        float sy = std::floor(inv_y + (18.0f + (i / 8) * 18.0f) * last_scale);

        if (x >= sx && x < sx + slot_size && y >= sy && y < sy + slot_size) {
            if (!cursor_item.is_empty()) {
                if (it != g_global_vars->items.end() && cursor_item.id == it->second->numeric_id) {
                    int max_stack = cursor_item.get_ptr() ? cursor_item.get_ptr()->max_count : it->second->max_count;
                    int add = (shift_held || button == ImGuiKey_MouseRight) ? max_stack : 1;
                    cursor_item = c_inventory_item(it->second->numeric_id, std::min(max_stack, cursor_item.count + add));
                }
                else {
                    cursor_item.reset();
                }
            }
            else if (it != g_global_vars->items.end()) {
                int max_stack = it->second->max_count > 0 ? it->second->max_count : 1;
                int take_count = (shift_held || button == ImGuiKey_MouseRight) ? max_stack : 1;
                cursor_item = c_inventory_item(it->second->numeric_id, take_count);
            }
            return true;
        }

        if (it != g_global_vars->items.end())
            ++it;
    }

    slot_entry_t slots[INVENTORY_BAR_SIZE];
    int count = build_slots(&local->inventory, slots);

    for (int i = 0; i < count; ++i) {
        float sx = std::floor(inv_x + slots[i].x * last_scale);
        float sy = std::floor(inv_y + slots[i].y * last_scale);

        if (x >= sx && x < sx + slot_size && y >= sy && y < sy + slot_size) {
            handle_slot_click(&local->inventory, slots[i], button == ImGuiKey_MouseRight, shift_held);
            return true;
        }
    }

    if (!cursor_item.is_empty() && (x < inv_x || x >= inv_x + inv_w || y < inv_y || y >= inv_y + inv_h)) {
        cursor_item.reset();
        return true;
    }

    return false;
}

bool c_creative_inventory::on_mouse_scroll(int direction)
{
    int valid_items = 0;
    for (const auto& pair : g_global_vars->items) {
        if (pair.second && pair.second->numeric_id != 0)
            ++valid_items;
    }
    int total_rows = std::max(0, (valid_items + 7) / 8 - 8);

    if (total_rows > 0 && direction != 0) {
        float step = 1.0f / static_cast<float>(total_rows);
        scroll_progress -= (direction > 0 ? step : -step);
        scroll_progress = std::clamp(scroll_progress, 0.0f, 1.0f);
        return true;
    }

    return false;
}

void c_creative_inventory::render(float scale)
{
    last_scale = scale;

    c_player* local = g_global_vars->local_player;
    if (!local)
        return;

    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
    g_menu_draw->rect_filled(0.0f, 0.0f, display.width, display.height, IM_COL32(16, 16, 16, 192));

    float inv_w = 176.0f * scale;
    float inv_h = 208.0f * scale;
    float inv_x = std::floor((display.width - inv_w) * 0.5f);
    float inv_y = std::floor((display.height - inv_h) * 0.5f);

    g_menu_draw->sprite(mc_sprites::CREATIVE_BG, inv_x, inv_y, scale);
    g_menu_draw->text("Item selection", std::floor(inv_x + 8.0f * scale), std::floor(inv_y + 6.0f * scale), scale, false, IM_COL32(64, 64, 64, 255));

    int valid_items = 0;
    for (const auto& pair : g_global_vars->items) {
        if (pair.second && pair.second->numeric_id != 0)
            ++valid_items;
    }
    int total_rows = std::max(0, (valid_items + 7) / 8 - 8);
    float scroll = (total_rows > 0) ? std::clamp(scroll_progress, 0.0f, 1.0f) : 0.0f;

    float sb_x = std::floor(inv_x + 156.0f * scale);
    float sb_y = std::floor(inv_y + 18.0f * scale);

    float sb_h = 144.0f * scale;
    float thumb_h = 15.0f * scale;

    float thumb_y = std::floor(sb_y + scroll * (sb_h - thumb_h));

    g_menu_draw->sprite(mc_sprites::CREATIVE_SCROLL_THUMB, sb_x, thumb_y, scale);

    glm::vec2 mouse_pos = g_input->get_mouse_position();
    float slot_size = 16.0f * scale;

    int skip = static_cast<int>(scroll * total_rows + 0.5f) * 8;
    auto it = g_global_vars->items.begin();
    int skipped = 0;
    while (it != g_global_vars->items.end() && skipped < skip) {
        if (it->second && it->second->numeric_id != 0)
            ++skipped;
        ++it;
    }

    for (int i = 0; i < 64; ++i) {
        while (it != g_global_vars->items.end() && (!it->second || it->second->numeric_id == 0))
            ++it;

        float sx = std::floor(inv_x + (8.0f + (i % 8) * 18.0f) * scale);
        float sy = std::floor(inv_y + (18.0f + (i / 8) * 18.0f) * scale);

        if (it != g_global_vars->items.end()) {
            c_inventory_item temp(it->second->numeric_id, 1);
            render_slot_item(&temp, sx, sy, scale);
            ++it;
        }

        if (mouse_pos.x >= sx && mouse_pos.x < sx + slot_size &&
            mouse_pos.y >= sy && mouse_pos.y < sy + slot_size) {
            g_menu_draw->rect_filled(sx, sy, slot_size, slot_size, IM_COL32(255, 255, 255, 128));
        }
    }

    slot_entry_t slots[INVENTORY_BAR_SIZE];
    int count = build_slots(&local->inventory, slots);

    for (int i = 0; i < count; ++i) {
        float sx = std::floor(inv_x + slots[i].x * scale);
        float sy = std::floor(inv_y + slots[i].y * scale);

        render_slot_item(slots[i].item, sx, sy, scale);

        if (mouse_pos.x >= sx && mouse_pos.x < sx + slot_size &&
            mouse_pos.y >= sy && mouse_pos.y < sy + slot_size) {
            g_menu_draw->rect_filled(sx, sy, slot_size, slot_size, IM_COL32(255, 255, 255, 128));
        }
    }

    if (!cursor_item.is_empty()) {
        render_slot_item(&cursor_item, std::floor(mouse_pos.x - slot_size * 0.5f), std::floor(mouse_pos.y - slot_size * 0.5f), scale);
    }
}