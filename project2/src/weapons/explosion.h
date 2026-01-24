#pragma once
#include <SFML/Graphics.hpp>

class Explosion {
public:
	Explosion(const sf::Vector2f& pos, float radius, float duration = 0.5f);

	void update(float deltaTime);
	void draw(sf::RenderWindow& window);

	bool isFinished() const { return duration <= 0.f; }

private:
	sf::Vector2f position;
	float radius;
	float duration;
	float maxDuration;
};
