#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
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
	void updateAgentsState();
	void keepInsideWindow(GameObject& obj);
	void gameOver();

	void debug();
};
