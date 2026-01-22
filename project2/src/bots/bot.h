#pragma once
#include "../helpers/parameters.h"
#include "../components/fsm/fsm.h"
#include "../map/navigation_graph.h"
#include "../components/health/health.h"
#include "../weapons/rail_gun/rail_gun.h"
#include "../weapons/rocket_launcher/rocket_launcher.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Game;



class Bot {
public:
	Bot(int id, const sf::Vector2f& startPosition);

	Game* game;
	FSM fsm;
	Health health;
	RocketLauncher rocketLauncher;
	RailGun railGun;

	void setNavGraph(NavigationGraph* graph) { navGraph = graph; }
	void selectRandomNode();
	void setColor(const sf::Color& c) { color = c; }
	const sf::Color& getColor() const { return color; }
	const sf::Vector2f& getPosition() const { return position; }

	void followPath(float deltaTime);
	bool hasArrived() const;

	// pod debug ruchem gracza
	void playerControl(sf::Vector2f mousePos);
	// pomocnicze metody pod debug
	sf::Vector2f getClosestNodePosition(const sf::Vector2f& targetPos) const;

private:
	int id;
	sf::Vector2f position;
	sf::Color color = sf::Color::Green;

	NavigationGraph* navGraph = nullptr;
	std::vector<sf::Vector2f> currentPath;
	size_t currentPathIndex = 0;
};
