#pragma once
#include "menu.h"
#include "global_vars.h"
#include "input.h"
#include "block.h"
#include "world.h"

class c_game
{
public:
    std::mutex entity_update_mutex;

    void initialize();

    void initialize_render();
    void initialize_scene();

    void start();
    void update();
    void finish();
};

extern c_game* g_game;
