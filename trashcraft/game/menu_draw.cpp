#include "menu.h"

#include <raytrace/texture.h>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_freetype.h>

c_menu_draw* g_menu_draw = new c_menu_draw;

void c_menu_draw::initialize()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui::GetIO().IniFilename = NULL;

    ImFontConfig cfg;
	cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome;

    ImGui::GetIO().Fonts->AddFontFromFileTTF("./fonts/GUI.ttf", 36.0f, &cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
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

void c_menu_draw::text(const std::string& label, float x, float y, float scale, bool shadow, ImU32 color)
{
    if (label.empty())
        return;

    ImFont* font = ImGui::GetFont();
    if (shadow) {
        ImU32 r = ((color >> IM_COL32_R_SHIFT) & 0xFF) >> 2;
        ImU32 g = ((color >> IM_COL32_G_SHIFT) & 0xFF) >> 2;
        ImU32 b = ((color >> IM_COL32_B_SHIFT) & 0xFF) >> 2;
        ImU32 a = (color >> IM_COL32_A_SHIFT) & 0xFF;
        ImGui::GetBackgroundDrawList()->AddText(font, 9.0f * scale, ImVec2(x + scale, y + scale), IM_COL32(r, g, b, a), label.c_str());
    }
    ImGui::GetBackgroundDrawList()->AddText(font, 9.0f * scale, ImVec2(x, y), color, label.c_str());
}

c_menu_draw::rect_size_t c_menu_draw::screen_size()
{
    ImVec2 size = ImGui::GetIO().DisplaySize;
    return { size.x, size.y };
}

c_menu_draw::rect_size_t c_menu_draw::text_size(const std::string& label, float scale)
{
    if (label.empty())
        return { 0.0f, 0.0f };

    ImVec2 size = ImGui::CalcTextSize(label.c_str());
	return { size.x * scale, size.y * scale };
}

void c_menu_draw::sprite(c_texture* texture, float x, float y, float u, float v, float w, float h, float scale, ImU32 color)
{
    if (!texture) {
        return;
    }

    ImVec2 p_min(x, y);
    ImVec2 p_max(x + w * scale, y + h * scale);

    ImVec2 uv_min(u / texture->size.x, v / texture->size.x);
    ImVec2 uv_max((u + w) / texture->size.x, (v + h) / texture->size.x);

    ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)texture->id, p_min, p_max, uv_min, uv_max, color);
}

void c_menu_draw::sprite(const std::string& name, float x, float y, float u, float v, float w, float h, float scale, ImU32 color)
{
    if (name.empty()) {
        return;
    }

    sprite(g_texture_manager->find_texture_by_name(name), x, y, u, v, w, h, scale, color);
}

void c_menu_draw::sprite(const gui_sprite_t& sprite, float x, float y, float scale, ImU32 color)
{
    this->sprite(sprite.atlas, x, y, sprite.u, sprite.v, sprite.w, sprite.h, scale, color);
}

void c_menu_draw::draw_iso_block(c_block* block, float x, float y, float size)
{
    ImVec2 center(x + size * 0.5f, y + size * 0.5f);

    float h = size * 0.48f;

    float rx = size * 0.45f;
    float ry = size * 0.25f;

    ImVec2 top_t(center.x, center.y - h);
    ImVec2 top_r(center.x + rx, center.y - h + ry);
    ImVec2 top_b(center.x, center.y - h + ry * 2.0f);
    ImVec2 top_l(center.x - rx, center.y - h + ry);

    ImVec2 bot_r(center.x + rx, center.y + ry);
    ImVec2 bot_b(center.x, center.y + ry * 2.0f);
    ImVec2 bot_l(center.x - rx, center.y + ry);

    c_texture* tex_top = block->materials[c_block::UPPER] ? block->materials[c_block::UPPER] : block->materials[c_block::SIDE];
    c_texture* tex_side = block->materials[c_block::SIDE];

    if (tex_top) {
        ImGui::GetBackgroundDrawList()->AddImageQuad((ImTextureID)(intptr_t)tex_top->id, top_l, top_t, top_r, top_b, { 0, 1 }, { 0, 0 }, { 1, 0 }, { 1, 1 }, IM_COL32(255, 255, 255, 255));
    }

    if (tex_side) {
        ImGui::GetBackgroundDrawList()->AddImageQuad((ImTextureID)(intptr_t)tex_side->id, top_l, top_b, bot_b, bot_l, { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, IM_COL32(205, 205, 205, 255));
        ImGui::GetBackgroundDrawList()->AddImageQuad((ImTextureID)(intptr_t)tex_side->id, top_b, top_r, bot_r, bot_b, { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, IM_COL32(155, 155, 155, 255));
    }
}