#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

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
	virtual void shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots);
	virtual void addAmmo(int amount) = 0;
	void setAmmo(int amount) { ammo = amount; }
	int getAmmo() const { return ammo; }
	void reloading(float deltaTime);
	bool canShoot() const { return ammo > 0 && available <= 0.f; }
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual void update(float deltaTime);
};
