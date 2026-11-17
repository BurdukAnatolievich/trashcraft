#include "inventory_overlay.h"
#include "menu.h"
#include "global_vars.h"
#include "entity/entity_list.h"

#include <algorithm>

void c_inventory_overlay::update()
{

}

void c_inventory_overlay::render(float scale)
{
    c_player* local = g_global_vars->local_player;
    if (!local) {
        return;
    }


}