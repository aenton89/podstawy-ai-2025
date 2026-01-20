#pragma once

#include <random>
#include <SFML/Graphics/RenderWindow.hpp>
#include "../map/map.h"
#include "../map/navigation_graph.h"
#include "../bots/bot.h"


class Game {
public:
	Game();
	void run();

	// mapa i graf odpowiadający za nawigacje
	std::unique_ptr<Map> map;
	std::unique_ptr<NavigationGraph> navGraph;

	// spawnowane boty
	std::vector<std::unique_ptr<Bot>> bots;
	// spawnpointy (idk czy tu)
	std::vector<sf::Vector2f> spawnPoints = {
		{100.f, 100.f},
		{1500.f, 100.f},
		{100.f, 800.f},
		{1500.f, 800.f}
	};

	// generator liczb losowych w sumie lepiej tu trzymać, bo jest często wywolywany
	static std::mt19937 gen;

private:
	sf::RenderWindow window;
	sf::Clock clock;

	// TODO: do debugowania
	bool showGraph = true;
	bool showNodes = true;
	bool showEdges = true;

	void processEvents();
	void update(float deltaTime);
	void render();

	void generateMap();
	void spawnBots();
	void deleteDeadBots();
	void gameOver();

	void debug() const;
};
