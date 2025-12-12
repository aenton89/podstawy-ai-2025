#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "obstacle.h"


// mapa gry
class Map {
public:
	Map(float width, float height, float botRadius);

	void addObstacle(const std::vector<sf::Vector2f>& vertices);
	void generateDefaultObstacles();

	bool isPositionValid(const sf::Vector2f& pos, float radius) const;
	bool isPathClear(const sf::Vector2f& from, const sf::Vector2f& to, float radius) const;

	void draw(sf::RenderWindow& window) const;

	float getWidth() const { return width; }
	float getHeight() const { return height; }
	float getBotRadius() const { return botRadius; }
	const std::vector<Obstacle>& getObstacles() const { return obstacles; }

private:
	float width;
	float height;
	float botRadius;
	std::vector<Obstacle> obstacles;
	sf::RectangleShape border;
};