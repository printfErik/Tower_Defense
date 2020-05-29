#include "EconSystem.h"

EconSystem::EconSystem()
{
	level = 1;
	global_net = 100000;
	tower1_cost = 200;
	obstacle_cost = 100;
	enemy_destroy_earn = 50 * 2 * level;
	enemy_damage = 50;
	player_health = 1000;
	upgrade_tower1 = 200;
}