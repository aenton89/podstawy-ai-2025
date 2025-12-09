#pragma once

#include <random>
#include <SFML/Graphics/RenderWindow.hpp>


class Game {
public:
	Game();
	void run();

	// TODO: tu referencja do mapy i botów


	// generator liczb losowych w sumie lepiej tu trzymać, bo jest często wywolywany
	static std::mt19937 gen;

private:
	sf::RenderWindow window;
	sf::Clock clock;

	void processEvents();
	void update(float deltaTime);
	void render();

	void generateMap();
	void spawnBots();
	void deleteDeadBots();
	void gameOver();

	void debug() const;
};
