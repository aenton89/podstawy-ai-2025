#pragma once
#include "../helpers/parameters.h"
#include "../components/fsm/fsm.h"



class Bot {
	int health = MAX_HEALTH;
	int rail_gun_ammo = RAIL_GUN_AMMO_CAPACITY;
	int rocket_launcher_ammo = ROCKET_LAUNCHER_AMMO_CAPACITY;

	FSM fsm;
};
