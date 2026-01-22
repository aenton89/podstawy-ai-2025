#include "rail_gun.h"



RailGun::RailGun() {
	damage = Parameters::RAILGUN_DAMAGE;
	ammo = Parameters::RAILGUN_MAX_AMMO;
	cooldown = Parameters::RAILGUN_RELOAD_TIME;
}

void RailGun::addAmmo(int amount) {
	ammo += amount;
	if (ammo > Parameters::RAILGUN_MAX_AMMO) {
		ammo = Parameters::RAILGUN_MAX_AMMO;
	}
}

void RailGun::shoot() {
	if (!canShoot())
		return;

	Weapon::shoot();

	// TODO: zaimplementować mechanike
}