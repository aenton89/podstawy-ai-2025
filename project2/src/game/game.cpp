#include "game.h"

#include <iostream>

#include "../helpers/parameters.h"
#include <SFML/Graphics.hpp>


std::mt19937 Game::gen{ std::random_device{}() };

Game::Game(): window(sf::VideoMode(1600, 900), "ASSIGNMENT 2: death match") {
	sf::Vector2u winSize = window.getSize();
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
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			window.close();
	}
}

void Game::update(float deltaTime) {
	// TODO: update botów i spawn heal'ów/amunicji


	deleteDeadBots();
}

void Game::render() {
	window.clear();

	// TODO: rysowanie mapy i botów


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
