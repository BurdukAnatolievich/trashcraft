#include "inventory_screen.h"
#include "menu.h"
#include "global_vars.h"
#include "entity/entity_list.h"

#include <algorithm>

void c_inventory_screen::update()
{
    c_player* local = g_global_vars->local_player;
    if (!local) {
        return;
    }

}

void c_inventory_screen::handle_slot_click(c_inventory* inventory, slot_entry_t& slot, bool right_click)
{
    c_inventory_item* target = slot.item;

    if (slot.is_product) {
        if (target->is_empty())
            return;

        bool can_take = cursor_item.is_empty() ||
            (cursor_item.id == target->id && cursor_item.usage_count == target->usage_count && cursor_item.count + target->count <= 64);

        if (!can_take)
            return;

        if (cursor_item.is_empty()) {
            cursor_item = *target;
        }
        else {
            cursor_item.count += target->count;
        }

        for (int i = 0; i < 4; ++i) {
            if (!inventory->crafting_grid_items[i].is_empty())
                inventory->crafting_grid_items[i].decrement_count(1);
        }

        target->reset();
        return;
    }

    if (cursor_item.is_empty()) {
        if (target->is_empty())
            return;

        if (right_click) {
            uint16_t take = (target->count + 1) / 2;
            cursor_item = *target;
            cursor_item.count = take;
            target->decrement_count(take);
        }
        else {
            cursor_item = *target;
            target->reset();
        }
    }
    else {
        if (target->is_empty()) {
            if (right_click) {
                *target = cursor_item;
                target->count = 1;
                cursor_item.decrement_count(1);
            }
            else {
                *target = cursor_item;
                cursor_item.reset();
            }
        }
        else if (target->id == cursor_item.id && target->usage_count == cursor_item.usage_count) {
            uint16_t max_stack = 64;
            uint16_t space = (target->count < max_stack) ? (max_stack - target->count) : 0;
            if (space > 0) {
                uint16_t move = right_click ? 1 : (std::min)(space, cursor_item.count);
                target->count += move;
                cursor_item.decrement_count(move);
            }
        }
        else if (!right_click) {
            std::swap(*target, cursor_item);
        }
    }
}

void c_inventory_screen::render_slot_item(c_inventory_item* item, float x, float y, float scale)
{
    if (item->is_empty())
        return;

    float size = 16.0f * scale;

    c_item* item_ptr = item->get_ptr();
    if (item_ptr) {
        if (item_ptr->type == c_item::e_type::BLOCK) {
            g_menu_draw->draw_iso_block(static_cast<c_block*>(item_ptr), x, y, size);
        }
        else if (item_ptr->item_material) {
            g_menu_draw->sprite(item_ptr->item_material, x, y, 0.0f, 0.0f, 1.0f, 1.0f, scale);
        }
    }

    if (item->count > 1) {
        std::string count_str = std::to_string(item->count);

		c_menu_draw::rect_size_t text_size = g_menu_draw->text_size(count_str, scale);
        float tx = std::floor(x + size + 1.0f * scale - text_size.width);
        float ty = std::floor(y + size + 1.0f * scale - text_size.height);

        g_menu_draw->text(count_str, tx, ty, scale);
    }
}

int c_inventory_screen::build_slots(c_inventory* inventory, slot_entry_t* out_slots)
{
    int count = 0;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 9; col++) {
            out_slots[count++] = { inventory->get_item(row, col), 8.0f + col * 18.0f, 84.0f + row * 18.0f, false };
        }
    }

    for (int col = 0; col < 9; col++) {
        out_slots[count++] = { inventory->get_item(e_inventory_bar::BAR_HOTBAR, col), 8.0f + col * 18.0f, 142.0f, false };
    }

    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 2; col++) {
            out_slots[count++] = { inventory->get_crafting_item(row, col), 98.0f + col * 18.0f, 18.0f + row * 18.0f, false };
        }
    }

    out_slots[count++] = { &inventory->crafting_product, 154.0f, 28.0f, true };
    return count;
}

bool c_inventory_screen::on_key_press(int key)
{
    return false;
}

bool c_inventory_screen::on_mouse_click(int button, float x, float y)
{
    if (button != ImGuiKey_MouseLeft && button != ImGuiKey_MouseRight)
        return false;

    c_player* local = g_global_vars->local_player;
    if (!local)
        return false;

    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();
    float inv_x = std::floor((display.width - 176.0f * last_scale) * 0.5f);
    float inv_y = std::floor((display.height - 166.0f * last_scale) * 0.5f);
    float slot_size = 16.0f * last_scale;

    slot_entry_t slots[INVENTORY_BAR_SIZE * (INVENTORY_BAR_COUNT + 1) + 4 + 1];
    int count = build_slots(&local->inventory, slots);

    for (int i = 0; i < count; ++i) {
        float sx = inv_x + slots[i].x * last_scale;
        float sy = inv_y + slots[i].y * last_scale;

        if (x >= sx && x < sx + slot_size && y >= sy && y < sy + slot_size) {
            handle_slot_click(&local->inventory, slots[i], button == ImGuiKey_MouseRight);
            return true;
        }
    }

    return false;
}

void c_inventory_screen::render(float scale)
{
    last_scale = scale;

    c_player* local = g_global_vars->local_player;
    if (!local) {
        return;
    }

    ImVec2 mouse_pos = ImGui::GetIO().MousePos;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    c_menu_draw::rect_size_t display = g_menu_draw->screen_size();

    draw->AddRectFilled({ 0.0f, 0.0f }, { display.width, display.height }, IM_COL32(16, 16, 16, 192));

    float inv_w = 176.0f * scale;
    float inv_h = 166.0f * scale;
    float inv_x = std::floor((display.width - inv_w) * 0.5f);
    float inv_y = std::floor((display.height - inv_h) * 0.5f);

    g_menu_draw->sprite(mc_sprites::INVENTORY_BG, inv_x, inv_y, scale);
    g_menu_draw->text("lang_inventory_crafting", std::floor(inv_x + 97.0f * scale), std::floor(inv_y + 8.0f * scale), scale, IM_COL32(64, 64, 64, 255));

    slot_entry_t slots[INVENTORY_BAR_SIZE * (INVENTORY_BAR_COUNT + 1) + 4 + 1];
    int count = build_slots(&local->inventory, slots);

    float slot_size = 16.0f * scale;
    for (int i = 0; i < count; ++i) {
        float sx = inv_x + slots[i].x * scale;
        float sy = inv_y + slots[i].y * scale;

        if (mouse_pos.x >= sx && mouse_pos.x < sx + slot_size &&
            mouse_pos.y >= sy && mouse_pos.y < sy + slot_size) {
            draw->AddRectFilled({ sx, sy }, { sx + slot_size, sy + slot_size }, IM_COL32(255, 255, 255, 128));
        }

        render_slot_item(slots[i].item, sx, sy, scale);
    }

    if (!cursor_item.is_empty()) {
        render_slot_item(&cursor_item, mouse_pos.x - slot_size * 0.5f, mouse_pos.y - slot_size * 0.5f, scale);
    }
}