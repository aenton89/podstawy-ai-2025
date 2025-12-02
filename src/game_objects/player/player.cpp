#include "player.h"
#include "../../helpers/parameters.h"
#include <numbers>
#include <cmath>
#include <SFML/Graphics.hpp>



Player::Player(float _speed, Game* _game): GameObject(0.f, 0.f, PLAYER_SIZE), speed(_speed) {
	shape = sf::CircleShape(PLAYER_SIZE, 3);
	shape.setFillColor(sf::Color::Red);
	shape.setOrigin(shape.getRadius(), shape.getRadius());

	game = _game;
	velocity = sf::Vector2f(0.f, 0.f);
	previousPosition = sf::Vector2f(0.f, 0.f);
}

void Player::handleInput(float deltaTime) {
	sf::Vector2f movement(0.f, 0.f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		movement.y -= speed * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		movement.y += speed * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		movement.x -= speed * deltaTime;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		movement.x += speed * deltaTime;

	shape.move(movement);

	// aktualizujemy pozycję collidera
	sf::Vector2f pos = shape.getPosition();
	collider.position.x = pos.x;
	collider.position.y = pos.y;
}

void Player::updateRotation(const sf::RenderWindow& window) {
	sf::Vector2f pos = shape.getPosition();
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	float dx = mousePos.x - pos.x;
	float dy = mousePos.y - pos.y;
	float angle = std::atan2(dy, dx);

	shape.setRotation(angle * 180 / std::numbers::pi + 90.f);
}

void Player::updateColliderPosition() {
	collider.position = { shape.getPosition().x, shape.getPosition().y };
}

void Player::updateVelocity(float deltaTime) {
	if (deltaTime > 0.f) {
		sf::Vector2f currentPosition = shape.getPosition();
		velocity = (currentPosition - previousPosition) / deltaTime;
		previousPosition = currentPosition;
	}
}

void Player::update(float dt, sf::RenderWindow &window) {
	handleInput(dt);
	updateRotation(window);
	updateColliderPosition();
	updateVelocity(dt);
}

sf::Vector2f Player::getForwardDirection(const sf::RenderWindow& window) const {
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f dir(mousePos.x - shape.getPosition().x, mousePos.y - shape.getPosition().y);
	float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	if (length != 0)
		dir /= length;
	return dir;
}
