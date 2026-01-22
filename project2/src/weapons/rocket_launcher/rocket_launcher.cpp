#include "rocket_launcher.h"



RocketLauncher::RocketLauncher() {
	damage = Parameters::ROCKET_DAMAGE;
	ammo = Parameters::ROCKET_MAX_AMMO;
	cooldown = Parameters::ROCKET_RELOAD_TIME;
}

void RocketLauncher::addAmmo(int amount) {
	ammo += amount;
	if (ammo > Parameters::ROCKET_MAX_AMMO) {
		ammo = Parameters::ROCKET_MAX_AMMO;
	}
}

void RocketLauncher::shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) {
	if (!canShoot())
		return;

	Weapon::shoot(shooterPos, direction, shooter, allBots);

	// TODO: zaimplementować mechanike
}