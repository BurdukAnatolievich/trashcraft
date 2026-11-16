#pragma once

class c_gui_screen {
public:
    virtual ~c_gui_screen() = default;

    virtual void update() = 0;
    virtual void render(float scale) = 0;

    virtual bool on_mouse_click(int button, float x, float y) { return false; };
    virtual bool on_key_press(int key) { return false; };

    virtual bool pauses_game() const { return false; };
    virtual bool is_full_screen() const { return false; }
    virtual bool can_close_with_esc() const { return true; }
};