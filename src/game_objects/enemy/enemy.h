#pragma once
#include "../game_object.h"
#include "../../components/steering_behaviours/steering_behaviours.h"
#include "../player/player.h"


// TODO: do stanów
enum State {
	Hide_Explore,
	Attack
};

class Enemy : public GameObject {
public:
	Player* player;
	sf::Vector2f velocity;
	// zakładamy, że masa jest jednostkowa, więc jest to nasze przyśpieszenie
	sf::Vector2f steering_force;
	// te dwa są potrzebne do paru funkcji w steering behaviours
	sf::Vector2f heading;
	sf::Vector2f side;
	bool was_hit = false;
	// komponent zachowań sterujących - do konstruktora przyjmuje this
	SteeringBehaviours steering;
	// dla zachowań grupowych
	bool tagged = false;
	// TODO: stany
	State currentState = State::Hide_Explore;

	Enemy(float _x, float _y, Player* _player);

	void updateColliderPosition();
	void steeringBehavior();

	void tag();
	void unTag();
	const bool isTagged() const;

	void setState(State state);
	State getState() const;
	std::vector<Enemy*> checkNeighborExploring(const std::vector<std::unique_ptr<Enemy>>& neighbors, float radius);


	void update(float dt, sf::RenderWindow& window) override;
};
