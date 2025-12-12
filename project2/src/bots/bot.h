#pragma once
#include "../helpers/parameters.h"
#include "../components/fsm/fsm.h"



class Bot {
	int health = Parameters::BOT_MAX_HEALTH;
	int rail_gun_ammo = Parameters::RAILGUN_MAX_AMMO;
	int rocket_launcher_ammo = Parameters::ROCKET_MAX_AMMO;

	FSM fsm;
};
