#pragma once

#include <vector>
#include <SFML/Graphics.hpp>



// klasa reprezentująca przeszkodę na mapie (zamknięty wielokąt)
class Obstacle {
public:
	Obstacle(const std::vector<sf::Vector2f>& vertices);

	bool containsPoint(const sf::Vector2f& point) const;
	bool intersectsCircle(const sf::Vector2f& center, float radius) const;
	void draw(sf::RenderWindow& window) const;

	const std::vector<sf::Vector2f>& getVertices() const { return vertices; }

private:
	std::vector<sf::Vector2f> vertices;
	sf::ConvexShape shape;

	bool pointInPolygon(const sf::Vector2f& point) const;
	float distanceToSegment(const sf::Vector2f& point, const sf::Vector2f& a, const sf::Vector2f& b) const;
};