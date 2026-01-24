#include "explosion.h"

Explosion::Explosion(const sf::Vector2f& pos, float radius, float duration)
	: position(pos), radius(radius), duration(duration), maxDuration(duration) {
}

void Explosion::update(float deltaTime) {
	duration -= deltaTime;
}

void Explosion::draw(sf::RenderWindow& window) {
	if (duration <= 0.f)
		return;

	// fading wybuchu
	float alpha = (duration / maxDuration) * 150.f;

	// rysuj wybuch
	sf::CircleShape explosionRadiusCircle(radius);
	explosionRadiusCircle.setPosition(position.x - radius, position.y - radius);
	explosionRadiusCircle.setFillColor(sf::Color::Transparent);
	explosionRadiusCircle.setOutlineThickness(5.f);
	explosionRadiusCircle.setOutlineColor(sf::Color(255, 165, 0, static_cast<sf::Uint8>(alpha)));
	window.draw(explosionRadiusCircle);
}
