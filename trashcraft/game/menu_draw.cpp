#include "menu.h"

#include <raytrace/texture.h>

#include <imgui.h>
#include <imgui_stdlib.h>

c_menu_draw* g_menu_draw = new c_menu_draw;

void c_menu_draw::initialize()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui::GetIO().IniFilename = NULL;

    ImFontConfig cfg;
    cfg.OversampleH = cfg.OversampleV = 1;
    cfg.PixelSnapH = true;

    ImGui::GetStyle().AntiAliasedLinesUseTex = false;
    ImGui::GetIO().Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;

    ImGui::GetIO().Fonts->AddFontFromFileTTF("./fonts/GUI.ttf", 18.0f, &cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
}

bool c_menu_draw::button(const std::string& label, float x, float y, float w, float h, float scale, bool disabled)
{
    ImVec2 mouse = ImGui::GetMousePos();
    bool hovered = !disabled && ImGui::IsMouseHoveringRect(ImVec2(x, y), ImVec2(x + w, y + h), false);
    bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    const gui_sprite_t& btn_sprite = disabled ? mc_sprites::BUTTON_DISABLED : (hovered ? mc_sprites::BUTTON_HOVERED : mc_sprites::BUTTON_NORMAL);

    float half_w = w * 0.5f;
    float src_half_w = half_w / scale;

    sprite(btn_sprite.atlas, x, y, 0.0f, btn_sprite.v, src_half_w, btn_sprite.h, scale);
    sprite(btn_sprite.atlas, x + half_w, y, 200.0f - src_half_w, btn_sprite.v, src_half_w, btn_sprite.h, scale);

    ImU32 text_color = disabled ? IM_COL32(160, 160, 160, 255) : (hovered ? IM_COL32(255, 255, 160, 255) : IM_COL32(224, 224, 224, 255));

    ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(9.0f * scale, FLT_MAX, 0.0f, label.c_str());
    float tx = x + (w - text_size.x) * 0.5f;
    float ty = y + (h - text_size.y) * 0.5f;

    text(label, tx, ty, scale, text_color);
    return clicked;
}

void c_menu_draw::text(const std::string& label, float x, float y, float scale, ImU32 color)
{
    if (label.empty())
        return;

    ImU32 r = ((color >> IM_COL32_R_SHIFT) & 0xFF) >> 2;
    ImU32 g = ((color >> IM_COL32_G_SHIFT) & 0xFF) >> 2;
    ImU32 b = ((color >> IM_COL32_B_SHIFT) & 0xFF) >> 2;
    ImU32 a = (color >> IM_COL32_A_SHIFT) & 0xFF;
    ImU32 shadow_color = IM_COL32(r, g, b, a);

    ImFont* font = ImGui::GetFont();
    ImGui::GetBackgroundDrawList()->AddText(font, 9.0f * scale, ImVec2(x + scale, y + scale), shadow_color, label.c_str());
    ImGui::GetBackgroundDrawList()->AddText(font, 9.0f * scale, ImVec2(x, y), color, label.c_str());
}

void c_menu_draw::sprite(const std::string& name, float x, float y, float u, float v, float w, float h, float scale, ImU32 color)
{
    c_texture* texture = g_texture_manager->find_texture_by_name(name);
    if (!texture) {
        return;
    }

    ImVec2 p_min(x, y);
    ImVec2 p_max(x + w * scale, y + h * scale);

    ImVec2 uv_min(u / texture->size.x, v / texture->size.x);
    ImVec2 uv_max((u + w) / texture->size.x, (v + h) / texture->size.x);

    ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)texture->id, p_min, p_max, uv_min, uv_max, color);
}

void c_menu_draw::sprite(const gui_sprite_t& sprite, float x, float y, float scale, ImU32 color)
{
    this->sprite(sprite.atlas, x, y, sprite.u, sprite.v, sprite.w, sprite.h, scale, color);
}