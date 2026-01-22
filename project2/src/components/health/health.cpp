#include "health.h"



void Health::addHealth(int amount) {
	health += amount;
	if (health > Parameters::BOT_MAX_HEALTH) {
		health = Parameters::BOT_MAX_HEALTH;
	}
}

void Health::takeDamage(int amount) {
	health -= amount;
	if (health < 0) {
		health = 0;
	}
}
