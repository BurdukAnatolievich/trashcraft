#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "item.h"
#include "block.h"
#include "save.h"

#include "gui/hud.h"
#include "gui/debug_overlay.h"

#include <D3D11.h>

#include "imgui_internal.h"
#include "imgui.h"

extern ID3D11Device* g_pd3dDevice;

struct gui_sprite_t {
    const char* atlas;
    float u, v, w, h;
};

namespace mc_sprites {
    constexpr gui_sprite_t HOTBAR = { "widgets", 0, 0, 182, 22 };
    constexpr gui_sprite_t HOTBAR_SELECTOR = { "widgets", 0, 22, 24, 24 };

    constexpr gui_sprite_t BUTTON_DISABLED = { "widgets", 0, 46, 200, 20 };
    constexpr gui_sprite_t BUTTON_NORMAL = { "widgets", 0, 66, 200, 20 };
    constexpr gui_sprite_t BUTTON_HOVERED = { "widgets", 0, 86, 200, 20 };

    constexpr gui_sprite_t TITLE_MINE = { "title_logo", 0,  0, 155, 44 };
    constexpr gui_sprite_t TITLE_CRAFT = { "title_logo", 0, 45, 155, 44 };

    constexpr gui_sprite_t INVENTORY_BG = { "inventory", 0, 0, 176, 166 };
    constexpr gui_sprite_t CRAFTING_TABLE_BG = { "crafting_table", 0, 0, 176, 166 };

    constexpr gui_sprite_t FURNACE_BG = { "furnace", 0, 0, 176, 166 };
    constexpr gui_sprite_t FURNACE_FLAME = { "furnace", 176, 0, 14, 14 };
    constexpr gui_sprite_t FURNACE_ARROW = { "furnace", 176, 14, 24, 17 };

    constexpr gui_sprite_t CROSSHAIR = { "icons", 0, 0, 16, 16 };
    constexpr gui_sprite_t HEART_BG = { "icons", 16, 0, 9, 9 };
    constexpr gui_sprite_t HEART_FULL = { "icons", 52, 0, 9, 9 };
    constexpr gui_sprite_t HEART_HALF = { "icons", 61, 0, 9, 9 };
    constexpr gui_sprite_t ARMOR_BG = { "icons", 16, 9, 9, 9 };
    constexpr gui_sprite_t ARMOR_FULL = { "icons", 34, 9, 9, 9 };
    constexpr gui_sprite_t ARMOR_HALF = { "icons", 25, 9, 9, 9 };
    constexpr gui_sprite_t FOOD_BG = { "icons", 16, 27, 9, 9 };
    constexpr gui_sprite_t FOOD_FULL = { "icons", 52, 27, 9, 9 };
    constexpr gui_sprite_t FOOD_HALF = { "icons", 61, 27, 9, 9 };
    constexpr gui_sprite_t XP_BAR_BG = { "icons", 0, 64, 182, 5 };
    constexpr gui_sprite_t XP_BAR_FULL = { "icons", 0, 69, 182, 5 };
}

class c_menu_draw
{
public:
    struct rect_size_t {
        float width = 0.0f, height = 0.0f;
    };

    void initialize();

    bool button(const std::string& label, float x, float y, float w, float h, float scale = 1.0f, bool disabled = false);
    void text(const std::string& label, float x, float y, float scale = 1.0f, ImU32 color = IM_COL32_WHITE);

    rect_size_t screen_size();
    rect_size_t text_size(const std::string& label, float scale = 1.0f);

    void sprite(c_texture* texture, float x, float y, float u, float v, float w, float h, float scale = 1.0f, ImU32 color = IM_COL32_WHITE);
    void sprite(const std::string& name, float x, float y, float u, float v, float w, float h, float scale = 1.0f, ImU32 color = IM_COL32_WHITE);
    void sprite(const gui_sprite_t& sprite, float x, float y, float scale = 1.0f, ImU32 color = IM_COL32_WHITE);

    void draw_iso_block(c_block* block, float x, float y, float size);
};

class c_ui_manager {
public:
    c_ui_manager() {

    }

    ~c_ui_manager() {
        for (c_gui_screen* screen : screen_stack) {
            delete screen;
        }
        screen_stack.clear();
    }

    inline void push_screen(c_gui_screen* screen) {
        if (!screen)
            return;
        screen_stack.push_back(screen);
        update_cursor_state();
    }

    inline void pop_screen() {
        pending_pop = true;
    }

    inline void clear_screens() {
        pending_clear = true;
    }

    inline bool has_open_screen() const {
        return !screen_stack.empty();
    }

    inline c_gui_screen* get_active_screen() const {
        return screen_stack.empty() ? nullptr : screen_stack.back();
    }

    void on_escape_pressed();
    void update();
    void render();
    void process_pending_actions();
    void update_cursor_state();

    c_hud_overlay hud;
    c_debug_overlay debug_overlay;

    std::vector<c_gui_screen*> screen_stack;

    bool pending_pop = false;
    bool pending_clear = false;

    bool show_debug_f3 = false;
    bool hide_hud = false;
};

extern c_menu_draw* g_menu_draw;
extern c_ui_manager* g_ui_manager;