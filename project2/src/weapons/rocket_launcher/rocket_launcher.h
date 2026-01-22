#pragma once
#include "../weapon.h"
#include "../../helpers/parameters.h"



class RocketLauncher : public Weapon {
public:
	RocketLauncher();

	void shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) override;
	void addAmmo(int amount) override;
	void draw(sf::RenderWindow& window) override {};
	void update(float deltaTime) override {};
};
