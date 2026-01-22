#include "pickup.h"
#include "../helpers/parameters.h"
#include <cmath>



Pickup::Pickup(const sf::Vector2f& pos, PickupType t, const sf::Color& c): position(pos), type(t), color(c) {}

void Pickup::update(float deltaTime) {
	if (!active) {
		respawnTimer -= deltaTime;
		if (respawnTimer <= 0.f) {
			active = true;
			respawnTimer = 0.f;
		}
	}
}

void Pickup::draw(sf::RenderWindow& window) const {
	if (!active)
		return;

	sf::RectangleShape shape(sf::Vector2f(size, size));
	shape.setOrigin(size / 2.f, size / 2.f);
	shape.setPosition(position);
	shape.setFillColor(color);
	shape.setOutlineColor(sf::Color::White);
	shape.setOutlineThickness(2.f);
	window.draw(shape);
}

bool Pickup::checkCollision(const sf::Vector2f& botPos, float botRadius) {
	if (!active)
		return false;

	sf::Vector2f diff = botPos - position;
	float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

	// kolizja jeśli bot jest wystarczająco blisko
	if (distance < botRadius + size / 2.f) {
		active = false;
		respawnTimer = respawnTime;
		return true;
	}
	return false;
}



// HealthPack
HealthPack::HealthPack(const sf::Vector2f& pos): Pickup(pos, PickupType::HealthPack, sf::Color::Green) {}

void HealthPack::applyEffect(Bot* bot) {
	bot->health.addHealth(Parameters::HEALTH_PACK_AMOUNT);
}



// RailGunAmmoPack
RailGunAmmoPack::RailGunAmmoPack(const sf::Vector2f& pos): Pickup(pos, PickupType::RailGunAmmo, sf::Color::Yellow) {}

void RailGunAmmoPack::applyEffect(Bot* bot) {
	bot->railGun.addAmmo(Parameters::AMMO_PACK_RAILGUN_AMOUNT);
}



// RocketAmmoPack
RocketAmmoPack::RocketAmmoPack(const sf::Vector2f& pos): Pickup(pos, PickupType::RocketAmmo, sf::Color::Cyan) {}

void RocketAmmoPack::applyEffect(Bot* bot) {
	bot->rocketLauncher.addAmmo(Parameters::AMMO_PACK_ROCKET_AMOUNT);
}