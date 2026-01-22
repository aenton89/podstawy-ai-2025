#pragma once
#include "../weapon.h"
#include "../../helpers/parameters.h"



class RailGun : public Weapon {
public:
	RailGun();

	void shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) override;
	void addAmmo(int amount) override;
	void draw(sf::RenderWindow &window) override;
	void update(float deltaTime) override;

private:
	void checkHit(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots);

	// wizualizacja promienia
	bool beamActive = false;
	float beamTimer = 0.f;
	// jak długo widać promień (w sekundach)
	float beamDuration = 0.5f;
	sf::Vector2f beamStart;
	sf::Vector2f beamEnd;
};
