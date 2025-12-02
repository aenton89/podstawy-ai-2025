#include "obstacle.h"
#include <SFML/Graphics.hpp>



Obstacle::Obstacle(float _x, float _y, float _radius): GameObject(_x, _y, _radius) {
	shape.setRadius(_radius);
	shape.setOrigin(_radius, _radius);
	shape.setFillColor(sf::Color(100, 100, 100));
	shape.setPosition(_x, _y);
}