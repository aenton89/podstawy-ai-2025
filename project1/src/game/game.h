#pragma once

#include <vector>
#include <random>
#include "../game_objects/player/player.h"
#include "../game_objects/player/raycast.h"
#include "../game_objects/obstacle/obstacle.h"
#include "../game_objects/enemy/enemy.h"



class Game {
public:
	Game();
	void run();

	std::vector<std::unique_ptr<Obstacle>> obstacles;
	std::vector<std::unique_ptr<Enemy>> enemies;
	// generator liczb losowych w sumie lepiej tu trzymać, bo jest często wywolywany
	static std::mt19937 gen;

private:
	sf::RenderWindow window;
	sf::Clock clock;

	Player player;
	Raycast raycast;

	void processEvents();
	void update(float deltaTime);
	void render();

	void initObstacles(int count);
	void spawnEnemies(int max_amount);
	void deleteDeadEnemies();
	void updateAgentsState() const;
	void keepInsideWindow(GameObject& obj) const;
	void gameOver();

	void debug() const;
};
