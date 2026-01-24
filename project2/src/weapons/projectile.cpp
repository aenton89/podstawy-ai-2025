#include "projectile.h"
#include "explosion.h"
#include "../bots/bot.h"
#include "../map/map.h"
#include "../helpers/parameters.h"
#include <cmath>

Projectile::Projectile(const sf::Vector2f& startPos, const sf::Vector2f& direction, float speed, float damage, float explosionRadius, Bot* shooter, Map* map)
	: position(startPos), damage(damage), explosionRadius(explosionRadius), shooter(shooter), map(map), maxRange(Parameters::ROCKET_RANGE) {
	// normalizacja i prędkość
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if (length > 0) {
		velocity = sf::Vector2f((direction.x / length) * speed, (direction.y / length) * speed);
	}
}

void Projectile::update(float deltaTime, std::vector<std::unique_ptr<Bot>>* allBots, std::vector<std::unique_ptr<Explosion>>* explosions) {
	if (exploded)
		return;

	sf::Vector2f oldPos = position;
	position += velocity * deltaTime;

	float distTraveled = std::sqrt((position.x - oldPos.x) * (position.x - oldPos.x) + 
	                                (position.y - oldPos.y) * (position.y - oldPos.y));
	travelDistance += distTraveled;

	// czy za daleko
	if (travelDistance > maxRange) {
		explode(allBots, explosions);
		return;
	}

	// czy poza mapą
	if (position.x < 0 || position.x > Parameters::MAP_WIDTH ||
	    position.y < 0 || position.y > Parameters::MAP_HEIGHT) {
		explode(allBots, explosions);
		return;
	}

	checkCollisions(allBots, explosions);
}

void Projectile::checkCollisions(std::vector<std::unique_ptr<Bot>>* allBots, std::vector<std::unique_ptr<Explosion>>* explosions) {
	// sprawdź kolizje z przeszkodami
	if (map && map->isCircleCollidingWithObstacles(position, 3.f)) {
		explode(allBots, explosions);
		return;
	}

	// sprawdź kolizje z botami
	for (auto& bot : *allBots) {
		if (bot.get() == shooter)
			continue;

		sf::Vector2f toBot = bot->getPosition() - position;
		float distToBot = std::sqrt(toBot.x * toBot.x + toBot.y * toBot.y);

		if (distToBot < Parameters::BOT_RADIUS) {
			explode(allBots, explosions);
			return;
		}
	}
}

void Projectile::explode(std::vector<std::unique_ptr<Bot>>* allBots, std::vector<std::unique_ptr<Explosion>>* explosions) {
	exploded = true;

	std::cout << "Rocket exploded at (" << position.x << ", " << position.y << ")\n";

	// zadaj obrażenia w zasięgu
	for (auto& bot : *allBots) {
		if (bot.get() == shooter)
			continue;

		sf::Vector2f toBot = bot->getPosition() - position;
		float distToBot = std::sqrt(toBot.x * toBot.x + toBot.y * toBot.y);

		if (distToBot < explosionRadius) {
			bot->health.takeDamage(damage);
			std::cout << "Rocket explosion hit bot at distance " << distToBot << ", HP: " << bot->health.getHealth() << std::endl;
		}
	}

	// zespawnuj wybuch
	explosions->push_back(std::make_unique<Explosion>(position, explosionRadius));
}

void Projectile::draw(sf::RenderWindow& window) {
	if (exploded)
		return;

	// rysuj rakiete jako kółko
	sf::CircleShape rocket(5.f);
	rocket.setPosition(position.x - 5.f, position.y - 5.f);
	rocket.setFillColor(sf::Color::Yellow);
	window.draw(rocket);
}
