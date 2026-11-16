#pragma once
#include <mutex>
#include <algorithm>

#include <glm.hpp>

enum e_key_bindings
{
    BIND_MOVE_FORWARD,
    BIND_MOVE_BACKWARD,
    BIND_MOVE_RIGHT,
    BIND_MOVE_LEFT,
    BIND_JUMP,
    BIND_ATTACK,
    BIND_USE,
    BIND_INVENTORY,
    BIND_SPRINT,
    BIND_MAX = 200,
};

struct key_bind_t
{
    uint16_t key = 0;
    const char* name = nullptr;

    uint64_t last_press_tick = 0;
};

enum e_buttons
{
    IN_JUMP = (1 << 0),
    IN_MOVE_FORWARD = (1 << 1),
    IN_MOVE_LEFT = (1 << 2),
    IN_MOVE_RIGHT = (1 << 3),
    IN_MOVE_BACKWARD = (1 << 4),
    IN_SPRINT = (1 << 5),
    IN_ATTACK = (1 << 6),
    IN_ACTION = (1 << 7),
};

struct accumulated_input_t
{
    int buttons = 0;
    glm::vec2 angle_change = glm::vec2(0.0f);
};

class c_input
{
    bool is_locked = false;
    float time_since_last_input = 0.0f;

    glm::vec2 mouse_delta = glm::vec2(0.0f, 0.0f);
    glm::vec2 mouse_position = glm::vec2(0.0f, 0.0f);

    float mouse_scroll = 0.0f;

    std::mutex input_swap_mutex;
    accumulated_input_t accumulated_input = { 0, { 0.0f, 0.0f } };

    key_bind_t key_bindings[BIND_MAX] = { 0 };
public:
    c_input()
    {
        is_locked = false;
        time_since_last_input = 0.0f;

        mouse_delta = glm::vec2(0.0f, 0.0f);
        mouse_position = glm::vec2(0.0f, 0.0f);

        mouse_scroll = 0.0f;

        for (int i = 0; i < BIND_MAX; i++)
            key_bindings[i] = { 0, "lang_unknown", 0};
    }

    void initialize();
    void update();

    void handle_player_input();

    bool is_bind_pressed(uint8_t bind);
    bool is_bind_double_pressed(uint8_t bind);

    bool is_bind_down(uint8_t bind);
    bool is_bind_released(uint8_t bind);

    bool is_key_pressed(uint16_t key);
    bool is_key_down(uint16_t key);
    bool is_key_released(uint16_t key);

    inline float normalize_yaw(float yaw) {
        yaw = std::fmod(yaw + 180.0f, 360.0f);
        if (yaw < 0.0f) {
            yaw += 360.0f;
        }
        return yaw - 180.0f;
    }

    inline glm::vec2 clamp_angles(glm::vec2 angle) {
        return { std::clamp(angle.x, -89.0f, 89.0f), normalize_yaw(angle.y) };
    }
    
    inline void get_accumulated_input(accumulated_input_t& out) { /*std::lock_guard<std::mutex> input_guard(input_swap_mutex);*/ std::swap(out, accumulated_input); };

    inline void bind_key(uint8_t bind, uint16_t key) { this->key_bindings[bind].key; };
    inline const key_bind_t& get_key_bind(uint8_t bind) { return this->key_bindings[bind]; };

    inline void lock() { this->is_locked = true; };
    inline void unlock() { this->is_locked = false; };

    inline float get_mouse_scroll() { return mouse_scroll; };
    inline void set_mouse_scroll(float scroll) { this->mouse_scroll = scroll; };

    inline glm::vec2 get_mouse_delta() { return mouse_delta; };
    inline void set_mouse_delta(const glm::vec2& delta) { this->mouse_delta = delta; };

    inline glm::vec2 get_mouse_position() { return mouse_position; };
    inline void set_mouse_position(const glm::vec2& pos) { this->mouse_position = pos; };

    inline bool get_is_locked() { return is_locked; };
};

extern c_input* g_input;
