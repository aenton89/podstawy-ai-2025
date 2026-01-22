#pragma once
#include "../../helpers/parameters.h"



class Health {
private:
	int health = Parameters::BOT_MAX_HEALTH;

public:
	void addHealth(int amount);
	void takeDamage(int amount);
	int getHealth() const { return health; }
};