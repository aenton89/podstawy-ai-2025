#pragma once
#include <SFML/Graphics.hpp>
#include "../components/collision/collider.h"



class GameObject {
public:
	Collider collider;
	sf::CircleShape shape;

	explicit GameObject(float _x = 0, float _y = 0, float _r = 10);
	virtual ~GameObject() = default;

	virtual void update(float dt, sf::RenderWindow& window) {};
	void draw(sf::RenderWindow& window) const;

	sf::Vector2f getPosition() const;
	void setPosition(const sf::Vector2f& pos);
};
