#include "weapon.h"
#include "../bots/bot.h"



void Weapon::dealDamage(Bot* bot) {
	bot->health.takeDamage(damage);
}

void Weapon::shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) {
	ammo -= 1;
	available = cooldown;
}

void Weapon::reloading(float deltaTime) {
	if (available > 0.f)
		available -= deltaTime;
}

void Weapon::update(float deltaTime) {
	reloading(deltaTime);
}