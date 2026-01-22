#pragma once

class Bot;



// TODO: tu podstawy broni
class Weapon {
protected:
	float damage;
	int ammo;
	float cooldown;
	float available = 0.f;

public:
	void dealDamage(Bot* bot);
	virtual void shoot();
	virtual void addAmmo(int amount) = 0;
	int getAmmo() const { return ammo; }
	void reloading(float deltaTime);
	bool canShoot() const { return ammo > 0 && available <= 0.f; }
};
