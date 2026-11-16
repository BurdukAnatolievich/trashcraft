#include "time.h"

#include <Windows.h>

#include <iostream>
#include <thread>
#include <algorithm>

c_time* g_time = new c_time;

void c_time::start()
{
    LARGE_INTEGER li;

    QueryPerformanceFrequency(&li);
    timer_frequency = li.QuadPart / 1000.0;

    QueryPerformanceCounter(&li);
    timer_start = li.QuadPart;
}

void c_time::update()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);

    current_time_ms = double(li.QuadPart - timer_start) / timer_frequency;
    current_time = current_time_ms * 0.001;
}

void c_time::finish()
{
    timer_frequency = 0.0;
    timer_start = 0.0;

    current_time = 0.0;
    current_time_ms = 0.0;

    last_game_update_time = 0.0;

    tick_counter_update_time = 0.0;

    updated_ticks = 0;
    updated_ticks_last_second = 0;

    tick_count = 0;
    frame_count = 0;
}

bool c_time::can_run_update()
{
    return current_time >= (last_game_update_time + interval_per_tick);
}

void c_time::ran_update(bool frame)
{
    if (frame) {
        frame_count++;

        frame_time = current_time - last_frame_update_time;
        last_frame_update_time = current_time;

        interp_amount = std::clamp((current_time - last_game_update_time) / interval_per_tick, 0.0, 1.0);
        return;
    }

    last_game_update_time = current_time;

    interp_amount = std::clamp((current_time - last_game_update_time) / interval_per_tick, 0.0, 1.0);

    updated_ticks++;
    if (current_time - tick_counter_update_time >= 1.0)
    {
        tick_counter_update_time = current_time;
        updated_ticks_last_second = updated_ticks;
        updated_ticks = 0;
    }

    tick_count++;
}

void c_time::sleep_update()
{
    double next_tick = last_game_update_time + interval_per_tick;
    if (next_tick > current_time) {
        double time_to_sleep_ms = (next_tick - current_time) * 1000.0;
        if (time_to_sleep_ms > 4.0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(uint64_t(time_to_sleep_ms - 4.0)));
        }
        else {
            std::this_thread::yield();
        }
    }
}