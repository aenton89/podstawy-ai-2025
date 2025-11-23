#include "enemy.h"
#include "../../helpers/parameters.h"
#include <iostream>



Enemy::Enemy(float _x, float _y, Player* _player): GameObject(_x, _y, ENEMY_SIZE) {
	shape.setRadius(ENEMY_SIZE);
	shape.setOrigin(ENEMY_SIZE, ENEMY_SIZE);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition(_x, _y);

	steering.setParent(this);
	player = _player;
}

void Enemy::updateColliderPosition() {
	collider.position = { shape.getPosition().x, shape.getPosition().y };
}

void Enemy::update(float dt, sf::RenderWindow& window) {
	steering_force = steering.calculate();
	velocity += steering_force;

	// ograniczenie prędkości
	auto length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (length > MAX_ENEMY_SPEED)
		velocity = (velocity / length) * MAX_ENEMY_SPEED;

	// liczenie heading i side
	if (length > 0.0000001f)
		heading = normalizeVec2D(velocity);
	else
		heading = sf::Vector2f(1.f,0.f);
	side = sf::Vector2f(-heading.y, heading.x);

	shape.move(velocity*dt);
	updateColliderPosition();
}

void Enemy::tag() {
	tagged = true;
}

void Enemy::unTag() {
	tagged = false;
}

const bool Enemy::isTagged() const {
	return tagged;
}