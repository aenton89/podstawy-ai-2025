#pragma once

#include "../helpers/parameters.h"
#include "../components/fsm/fsm.h"
#include "../map/navigation_graph.h"
#include <SFML/Graphics.hpp>
#include <vector>



class Bot {
public:
	Bot(int id, const sf::Vector2f& startPosition);

	int health = Parameters::BOT_MAX_HEALTH;
	int rail_gun_ammo = Parameters::RAILGUN_MAX_AMMO;
	int rocket_launcher_ammo = Parameters::ROCKET_MAX_AMMO;

	FSM fsm;

	void setNavGraph(NavigationGraph* graph) { navGraph = graph; }
	void selectRandomNode();
	void setColor(const sf::Color& c) { color = c; }
	const sf::Color& getColor() const { return color; }
	const sf::Vector2f& getPosition() const { return position; }

	void followPath(float deltaTime);
	bool hasArrived() const;


private:
	int id;
	sf::Vector2f position;
	sf::Color color = sf::Color::Green;

	NavigationGraph* navGraph = nullptr;
	std::vector<sf::Vector2f> currentPath;
	size_t currentPathIndex = 0;
};
