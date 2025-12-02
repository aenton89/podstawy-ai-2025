#pragma once
#include "../game_object.h"
#include "../../components/steering_behaviours/steering_behaviours.h"
#include "../player/player.h"



// TODO: do stanów
enum State {
	HideExplore,
	Attack,
	RandomWander
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
	State currentState = State::HideExplore;
	// zegar, bo do randomWander wchodzimy losowo co jakiś czas
	sf::Clock randomWanderClock;
	float randomWanderDuration = 0.f;
	// poprzednia pozycja - też do randomWander
	sf::Vector2f prevPosition;
	// dzięki temu wiemy czy stoi w miejscu długo
	float idleTimer = 0.f;
	float idleTimeTreshold;

	Enemy(float _x, float _y, Player* _player);

	void updateColliderPosition();

	void tag();
	void unTag();
	bool isTagged() const;

	void setState(State state);
	State getState() const;
	std::vector<Enemy*> getNeighbours(const std::vector<std::unique_ptr<Enemy>>& neighbors, float radius) const;

	void randomWanderSwitch(float dt);
	bool checkRandomWanderTimeout();

	void update(float dt, sf::RenderWindow& window) override;
};
