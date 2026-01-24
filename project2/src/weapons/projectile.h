#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Bot;
class Explosion;
class Map;

class Projectile {
public:
	Projectile(const sf::Vector2f& startPos, const sf::Vector2f& direction, float speed, float damage, float explosionRadius, Bot* shooter, Map* map = nullptr);

	void update(float deltaTime, std::vector<std::unique_ptr<Bot>>* allBots, std::vector<std::unique_ptr<Explosion>>* explosions);
	void draw(sf::RenderWindow& window);

	bool hasExploded() const { return exploded; }
	const sf::Vector2f& getPosition() const { return position; }

private:
	sf::Vector2f position;
	sf::Vector2f velocity;
	float damage;
	float explosionRadius;
	Bot* shooter;
	Map* map;
	bool exploded = false;
	float travelDistance = 0.f;
	float maxRange = 300.f;

	void checkCollisions(std::vector<std::unique_ptr<Bot>>* allBots, std::vector<std::unique_ptr<Explosion>>* explosions);
	void explode(std::vector<std::unique_ptr<Bot>>* allBots, std::vector<std::unique_ptr<Explosion>>* explosions);
};
