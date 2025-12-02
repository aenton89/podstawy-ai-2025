#pragma once
#include "../game_object.h"

class Game;



class Player : public GameObject {
public:
	Game* game;
	float speed;
	// potrzebne było velocity pod evade()
	sf::Vector2f velocity;
	sf::Vector2f previousPosition;

	explicit Player(float _speed = 200.f, Game* _game = nullptr);

	void handleInput(float deltaTime);
	void updateRotation(const sf::RenderWindow& window);
	void updateColliderPosition();
	void updateVelocity(float deltaTime);

	void update(float dt, sf::RenderWindow& window) override;

	sf::Vector2f getForwardDirection(const sf::RenderWindow& window) const;
};
