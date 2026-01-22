#include "weapon.h"
#include "../bots/bot.h"



void Weapon::dealDamage(Bot* bot) {
	bot->health.takeDamage(damage);
}

void Weapon::shoot() {
	ammo -= 1;
	available = cooldown;
}

void Weapon::reloading(float deltaTime) {
	if (available > 0.f)
		available -= deltaTime;
}
