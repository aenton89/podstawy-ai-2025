#include "game.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "../helpers/parameters.h"


std::mt19937 Game::gen{ std::random_device{}() };

Game::Game(): window(sf::VideoMode(Parameters::MAP_WIDTH, Parameters::MAP_HEIGHT), "ASSIGNMENT 2: death match") {
	// to jest, żeby nie było zbyt szybko powtarzanych klawiszy - nie sterujemy postaciami więc można wyłączyć a toogle lepiej działa
	window.setKeyRepeatEnabled(false);
	sf::Vector2u winSize = window.getSize();

	generateMap();
}

void Game::run() {
	while (window.isOpen()) {
		processEvents();
		float deltaTime = clock.restart().asSeconds();
		update(deltaTime);
		render();
	}
}

void Game::processEvents() {
	sf::Event event{};
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape)
				window.close();

			// TODO: do debugowania
			if (event.key.code == sf::Keyboard::G)
				showGraph = !showGraph;
			if (event.key.code == sf::Keyboard::N)
				showNodes = !showNodes;
			if (event.key.code == sf::Keyboard::E)
				showEdges = !showEdges;
			if (event.key.code == sf::Keyboard::D)
				debug();
		}
	}
}

void Game::update(float deltaTime) {
	// TODO: update botów i spawn heal'ów/amunicji


	deleteDeadBots();
}

void Game::render() {
	window.clear();

	map->draw(window);

	// rysuj graf nawigacji - tylko jeśli jest włączony
	if (showGraph)
		navGraph->draw(window, showNodes, showEdges);


	window.display();
}

void Game::gameOver() {
	sf::Font font;
	if (!font.loadFromFile("../../../fonts/PublicSans-Black.ttf"))
		std::cerr << "ERR: can't load font!\n";

	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setString("GAME OVER");
	gameOverText.setCharacterSize(100);
	gameOverText.setFillColor(sf::Color::White);

	// wyśrodkowanie napisu
	sf::FloatRect textRect = gameOverText.getLocalBounds();
	gameOverText.setOrigin(textRect.width / 2, textRect.height / 2);
	gameOverText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

	// wyczyść ekran, narysuj napis i wyświetl
	window.clear(sf::Color::Black);
	window.draw(gameOverText);
	window.display();

	// zatrzymaj grę na chwilę i zamknij okno
	sf::sleep(sf::seconds(3));
	window.close();
}

void Game::deleteDeadBots() {
	// TODO: usuwanie martwych botów
	return;
}

void Game::spawnBots() {
	// TODO: spawn botów
	return;
}

void Game::generateMap() {
	map = std::make_unique<Map>(Parameters::MAP_WIDTH, Parameters::MAP_HEIGHT, Parameters::BOT_RADIUS);
	navGraph = std::make_unique<NavigationGraph>(map.get());

	// wygeneruj graf zaczynając od środka mapy
	sf::Vector2f startPoint(Parameters::MAP_WIDTH/2, Parameters::MAP_HEIGHT/2);

	// znajdź najbliższy wolny punkt jeśli środek jest zajęty
	if (!map->isPositionValid(startPoint, Parameters::BOT_RADIUS)) {
		// spróbuj kilka punktów wokół środka
		bool found = false;
		for (float r = 50.f; r < 400.f && !found; r += 50.f) {
			for (float angle = 0.f; angle < 360.f && !found; angle += 45.f) {
				float rad = angle * 3.14159f / 180.f;
				sf::Vector2f testPoint(
					800.f + r * std::cos(rad),
					450.f + r * std::sin(rad)
				);

				if (map->isPositionValid(testPoint, Parameters::BOT_RADIUS)) {
					startPoint = testPoint;
					found = true;
				}
			}
		}
	}

	std::cout << "Generating navigation graph..." << std::endl;
	navGraph->generateGraph(startPoint);
	std::cout << "Navigation graph generated with " << navGraph->getNodes().size() << " nodes" << std::endl;
}


void Game::debug() const {
	std::cout << "\n=== DEBUG INFO ===" << std::endl;

	if (map) {
		std::cout << "Map size: " << map->getWidth() << "x" << map->getHeight() << std::endl;
		std::cout << "Bot radius: " << map->getBotRadius() << std::endl;
		std::cout << "Obstacles: " << map->getObstacles().size() << std::endl;
	}
	if (navGraph)
		std::cout << "Graph nodes: " << navGraph->getNodes().size() << std::endl;

	std::cout << "==================\n" << std::endl;
}