#pragma once
#include <SFML/System/Vector2.hpp>



class Collider {
public:
	sf::Vector2f position;
	float radius;

	explicit Collider(float _x = 0, float _y = 0, float _r = 10);

	[[nodiscard]]
	bool checkCollision(const Collider& other) const;
};
