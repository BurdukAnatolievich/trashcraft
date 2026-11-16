#pragma once
#include <cstdint>

class c_time
{
    double timer_frequency = 0.0f;
    double timer_start = 0.0f;

    double current_time = 0.0f;
    double current_time_ms = 0.0f;

    double interval_per_tick = 0.0f;

    double last_game_update_time = 0.0f;
    double last_frame_update_time = 0.0f;

    double tick_counter_update_time = 0.0f;

    double frame_time = 0.0f;
    double interp_amount = 0.0f;

    uint64_t tick_count = 0;
    uint64_t frame_count = 0;

    uint16_t updated_ticks = 0;
    uint16_t updated_ticks_last_second = 0;
public:
    void start();
    void update();
    void finish();

    bool can_run_update();
    void ran_update(bool frame = false);

    void sleep_update();

    inline float get_interpolation_amount() { return interp_amount; };
    inline void set_interpolation_amount(float interp_amt) { interp_amount = interp_amt; };

    inline double get_last_game_update_time() { return last_game_update_time; }

    inline double get_interval_per_tick() { return interval_per_tick; }
    inline void set_interval_per_tick(double interval) { interval_per_tick = interval; }

    inline float get_current_time() { return current_time; }
    inline float get_current_time_ms() { return current_time_ms; }

    inline double get_last_frame_update_time() { return last_frame_update_time; };
    inline double get_frame_time() { return frame_time; };

    inline int get_updated_ticks() { return updated_ticks_last_second; }

    inline uint64_t get_frame_count() { return frame_count; };
    inline uint64_t get_tick_count() { return tick_count; };
};

extern c_time* g_time;
