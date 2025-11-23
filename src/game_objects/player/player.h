#pragma once
#include <SFML/Graphics.hpp>
#include "../game_object.h"

class Game;



class Player : public GameObject {
public:
	Game* game;
	float speed;
	// potrzebne było velocity pod evade()
	sf::Vector2f velocity;
	sf::Vector2f previousPosition;

	Player(float _speed = 200.f, Game* _game = nullptr);

	void handleInput(float deltaTime);
	void updateRotation(sf::RenderWindow& window);
	void updateColliderPosition();
	void updateVelocity(float deltaTime);

	void update(float dt, sf::RenderWindow& window) override;

	sf::Vector2f getForwardDirection(sf::RenderWindow& window) const;
};
