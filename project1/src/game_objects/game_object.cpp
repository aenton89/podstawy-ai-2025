#include "game_object.h"



GameObject::GameObject(float _x, float _y, float _r): collider(_x, _y, _r) {}

void GameObject::setPosition(const sf::Vector2f& pos) {
	collider.position = {pos.x, pos.y};
	shape.setPosition(pos);
}

sf::Vector2f GameObject::getPosition() const {
	return {collider.position.x, collider.position.y};
}

void GameObject::draw(sf::RenderWindow& window) const {
	window.draw(shape);
}