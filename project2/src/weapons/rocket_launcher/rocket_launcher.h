#pragma once
#include "../weapon.h"
#include "../../helpers/parameters.h"
#include "../projectile.h"
#include "../explosion.h"
#include <vector>
#include <memory>

class Map;

class RocketLauncher : public Weapon {
public:
	RocketLauncher();

	void shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) override;
	void addAmmo(int amount) override;
	void draw(sf::RenderWindow& window) override;
	void update(float deltaTime) override;
	void setMap(Map* mapPtr) { mapRef = mapPtr; }

	std::vector<std::unique_ptr<Projectile>>* getProjectiles() { return &projectiles; }
	std::vector<std::unique_ptr<Explosion>>* getExplosions() { return &explosions; }

private:
	std::vector<std::unique_ptr<Projectile>> projectiles;
	std::vector<std::unique_ptr<Explosion>> explosions;
	std::vector<std::unique_ptr<Bot>>* allBotsRef = nullptr;
	Map* mapRef = nullptr;
};
