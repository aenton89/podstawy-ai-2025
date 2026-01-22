#pragma once
#include <SFML/Graphics.hpp>
#include "../bots/bot.h"



enum class PickupType {
	HealthPack,
	RailGunAmmo,
	RocketAmmo
};



class Pickup {
protected:
	sf::Vector2f position;
	PickupType type;
	sf::Color color;
	float size = 20.f;
	bool active = true;
	// czas do respawnu w sekundach
	float respawnTime = 30.f;
	float respawnTimer = 0.f;

public:
	Pickup(const sf::Vector2f& pos, PickupType t, const sf::Color& c);
	virtual ~Pickup() = default;

	virtual void applyEffect(Bot* bot) = 0;
	void update(float deltaTime);
	void draw(sf::RenderWindow& window) const;

	bool checkCollision(const sf::Vector2f& botPos, float botRadius);
	bool isActive() const { return active; }
	const sf::Vector2f& getPosition() const { return position; }
	PickupType getType() const { return type; }
};



class HealthPack : public Pickup {
public:
	HealthPack(const sf::Vector2f& pos);
	void applyEffect(Bot* bot) override;
};



class RailGunAmmoPack : public Pickup {
public:
	RailGunAmmoPack(const sf::Vector2f& pos);
	void applyEffect(Bot* bot) override;
};



class RocketAmmoPack : public Pickup {
public:
	RocketAmmoPack(const sf::Vector2f& pos);
	void applyEffect(Bot* bot) override;
};