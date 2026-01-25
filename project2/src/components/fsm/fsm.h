#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../../helpers/parameters.h"



class Bot;
class Pickup;
class Obstacle;



enum class BotState {
	RoamRandomly,           // ma ammo i HP, szuka wroga
	LookForResources,       // ma malo ammo lub HP
	Fight,                  // znalazl wroga i ma zasoby
	FightToTheEnd,          // znalazl wroga ale malo zasobow i nie moze uciec
	RunAway                 // znalazl wroga ale brak zasobow
};


// kontekst zmyslow bota
struct BotSenseContext {
	Bot* detectedEnemy = nullptr;
	float enemyDistance = 0.f;
	bool enemyDetected = false;

	Pickup* detectedPickup = nullptr;
	float pickupDistance = 0.f;
	bool pickupDetected = false;

	bool hasManyResources = true;
	bool hasLowResources = false;
	bool hasNoAmmo = false;
	bool isEnemyCloseRange = false;
};



// bazowa klasa Stanu
class State {
public:
	virtual ~State() = default;
	virtual void enter(Bot* bot) {}
	virtual void exit(Bot* bot) {}
	virtual void update(Bot* bot, const BotSenseContext& senses, float deltaTime) = 0;
	virtual BotState getType() const = 0;
};



// stany
class RoamRandomlyState : public State {
public:
	void enter(Bot* bot) override;
	void update(Bot* bot, const BotSenseContext& senses, float deltaTime) override;
	BotState getType() const override { return BotState::RoamRandomly; }

private:
	float roamTimer = 0.f;
};



class LookForResourcesState : public State {
public:
	void enter(Bot* bot) override;
	void update(Bot* bot, const BotSenseContext& senses, float deltaTime) override;
	BotState getType() const override { return BotState::LookForResources; }

private:
	float searchTimer = 0.f;
	Pickup* targetPickup = nullptr;
};



class FightState : public State {
public:
	void enter(Bot* bot) override;
	void update(Bot* bot, const BotSenseContext& senses, float deltaTime) override;
	BotState getType() const override { return BotState::Fight; }

private:
	Bot* targetEnemy = nullptr;
	float pursuitTimer = 0.f;
};



class FightToTheEndState : public State {
public:
	void enter(Bot* bot) override;
	void update(Bot* bot, const BotSenseContext& senses, float deltaTime) override;
	BotState getType() const override { return BotState::FightToTheEnd; }

private:
	Bot* targetEnemy = nullptr;
	float targetLossTimer = 0.f;
};



class RunAwayState : public State {
public:
	void enter(Bot* bot) override;
	void update(Bot* bot, const BotSenseContext& senses, float deltaTime) override;
	BotState getType() const override { return BotState::RunAway; }

private:
	sf::Vector2f fleeDirection;
	sf::Vector2f lastFleeTarget;
	float fleeTimer = 0.f;
	Bot* threateningEnemy = nullptr;
};



// FSM
class FSM {
public:
	FSM();
	~FSM();

	void initialize(Bot* owner);
	void update(Bot* bot, float deltaTime);
	void setState(BotState newState);
	BotState getCurrentState() const;

	// metody pomocnicze
	BotSenseContext gatherSenses(Bot* bot) const;
	float distanceTo(const sf::Vector2f& from, const sf::Vector2f& to) const;
	sf::Vector2f directionTo(const sf::Vector2f& from, const sf::Vector2f& to) const;
	bool hasLineOfSight(const sf::Vector2f& from, const sf::Vector2f& to, Bot* ignoreBotSelf = nullptr) const;
    std::string getCurrentStateName() const;

private:
	BotState currentState = BotState::RoamRandomly;
	std::unique_ptr<State> stateImplementation;

	// metody pomocnicze
	bool hasManyResources(Bot* bot) const;
	bool hasLowResources(Bot* bot) const;
	bool hasNoAmmo(Bot* bot) const;
	bool canSeeEnemy(Bot* bot, Bot*& outEnemy, float& outDistance) const;
	Pickup* canSeePickup(Bot* bot, float& outDistance) const;
	bool isEnemyCloseRange(const Bot* enemy, float distance) const;

	// line of sight helpers
	bool lineIntersectsObstacle(const sf::Vector2f& from, const sf::Vector2f& to, const Obstacle& obstacle) const;
	bool lineSegmentsIntersect(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4) const;
	bool pointInPolygon(const sf::Vector2f& point, const std::vector<sf::Vector2f>& vertices) const;

	void createState(BotState state);
};