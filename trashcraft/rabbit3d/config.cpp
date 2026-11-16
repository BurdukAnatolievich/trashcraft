#include "config.h"

c_rabbit3d_config* g_config = new c_rabbit3d_config;

void c_rabbit3d_config::initialize() 
{
	screen_resolution = glm::vec2(1920, 1080);
}