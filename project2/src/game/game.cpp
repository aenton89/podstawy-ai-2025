#include "game.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "../helpers/parameters.h"


std::mt19937 Game::gen{ std::random_device{}() };

Game::Game(): window(sf::VideoMode(Parameters::MAP_WIDTH, Parameters::MAP_HEIGHT), "ASSIGNMENT 2: death match") {
	// to jest, żeby nie było zbyt szybko powtarzanych klawiszy - nie sterujemy postaciami więc można wyłączyć a toogle lepiej działa
	window.setKeyRepeatEnabled(false);
	sf::Vector2u winSize = window.getSize();

	// zaladuj font do statystyk botow
	if (!textFont.loadFromFile("../fonts/PublicSans-Black.ttf")) {
		std::cerr << "ERR: can't load font for stats!\n";
	}

	generateMap();
	spawnPickups();
	spawnBots();
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

			// Toggle debugujace:
			if (event.key.code == sf::Keyboard::P)		// sterowanie 1-ym botem przez gracza
				playerEnable = !playerEnable;
			if (event.key.code == sf::Keyboard::G)		// rysowanie grafu nawigacji
				showGraph = !showGraph;
			if (event.key.code == sf::Keyboard::N)		// rysowanie wezlow
				showNodes = !showNodes;
			if (event.key.code == sf::Keyboard::E)		// rysowanie krawedzi
				showEdges = !showEdges;
			if (event.key.code == sf::Keyboard::I)		// rysowanie statystyk botow
				showBotStats = !showBotStats;
			if (event.key.code == sf::Keyboard::C)		// rysowanie koordynatow
				showCoordinates = !showCoordinates;
			if (event.key.code == sf::Keyboard::D)		// debug w konsoli
				debug();
		}
	}
}

void Game::update(float deltaTime) {
	for (auto& bot : bots) {
		// tylko dla 1. bota
		if (playerEnable && bots.front() == bot) {
			sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
			sf::Vector2f pos(pixelPos.x, pixelPos.y);

			// poruszanie się jako gracz - debug
			bot->playerControl(pos);
			bot->followPath(deltaTime);
		}
		else {
			// AI bots use FSM for decision making
			bot->fsm.update(bot.get(), deltaTime);
		}

		// sprawdzenie czy trzeba przeładować i update stanu wizualizacji
		bot->railGun.update(deltaTime);
		bot->rocketLauncher.update(deltaTime);
	}

	// update pickupów i sprawdzenie kolizji
	updatePickups(deltaTime);

	respawnDeadBots();
}

void Game::render() {
	window.clear();

	map->draw(window);

	// rysuj graf nawigacji - tylko jeśli jest włączony
	if (showGraph)
		navGraph->draw(window, showNodes, showEdges);

	// rysuj pickupy
	for (const auto& pickup : pickups) {
		pickup->draw(window);
	}

	// rysuj boty
	sf::CircleShape botShape(Parameters::BOT_RADIUS);
	botShape.setOrigin(Parameters::BOT_RADIUS, Parameters::BOT_RADIUS);
	for (const auto& bot : bots) {
		botShape.setPosition(bot->getPosition());
		botShape.setFillColor(bot->getColor());
		window.draw(botShape);

		if (showBotStats) {
			// Pokaz satystyki bota nad nim
			sf::Text statsText;
			statsText.setFont(textFont);
			statsText.setCharacterSize(12);
			statsText.setFillColor(sf::Color::White);

			// Format:
			// HP | ammo railguna | ammo rakiet | stan FSM
			std::string statsString = std::to_string(static_cast<int>(bot->health.getHealth())) +
				" | " + std::to_string(bot->railGun.getAmmo()) +
				" | " + std::to_string(bot->rocketLauncher.getAmmo()) + 
				" | " + bot->fsm.getCurrentStateName();

			statsText.setString(statsString);
			
			// wyswietla tekst nad botem
			sf::FloatRect textBounds = statsText.getLocalBounds();
			statsText.setOrigin(textBounds.width / 2.f, textBounds.height + 5.f);
			statsText.setPosition(bot->getPosition().x, bot->getPosition().y - Parameters::BOT_RADIUS - 10.f);

			window.draw(statsText);
		}
	}

	// rysuj promienie railgunów
	for (const auto& bot : bots) {
		bot->railGun.draw(window);
		bot->rocketLauncher.draw(window);
	}

	// rysuj koordynaty na krawedziach mapy
	if (showCoordinates) {
		sf:: Text coordText;
		coordText.setFont(textFont);
		coordText.setCharacterSize(10);
		coordText.setFillColor(sf::Color::White);

		// x
		for (int x = 0; x <= Parameters::MAP_WIDTH; x += 100) {
			coordText.setString(std::to_string(x));
			coordText.setPosition(x - 5, 5); // top
			window.draw(coordText);
		}

		// y
		for (int y = 0; y <= Parameters::MAP_HEIGHT; y += 100) {
			coordText.setString(std::to_string(y));
			coordText.setPosition(5, y - 5); // left
			window.draw(coordText);
		}
	}

	// rysuj spawnpointy
	sf::CircleShape spawnShape(30.f);
	spawnShape.setOrigin(30.f, 30.f);

	for (const auto& spawnPoint : spawnPoints) {
		spawnShape.setPosition(spawnPoint);
		spawnShape.setFillColor(sf::Color(255, 255, 255, 40));
		window.draw(spawnShape);
	}

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

void Game::respawnDeadBots() {
	std::uniform_int_distribution<size_t> dist(0, spawnPoints.size() - 1);

	for (auto& bot : bots) {
		if (bot->health.getHealth() <= 0) {
			// reset bot's health, ammo and position
			bot->health.restore();
			bot->railGun.setAmmo(Parameters::RAILGUN_RESPAWN_AMMO);
			bot->rocketLauncher.setAmmo(Parameters::ROCKET_RESPAWN_AMMO);
			sf::Vector2f spawnPoint = spawnPoints[dist(gen)];
			bot->setPosition(spawnPoint);
			bot->selectRandomNode();
			bot->fsm.setState(BotState::RoamRandomly);

			std::cout << "Bot " << bot->getId() << " respawned at (" << spawnPoint.x << ", " << spawnPoint.y << ")" << std::endl;
		}
	}
}

void Game::spawnBots() {
	const sf::Color botColors[] = {
		sf::Color::Red,
		sf::Color::Blue,
		sf::Color::Green,
		sf::Color::Magenta
	};

	// spawnuj spawny
	spawnPoints = std::vector<sf::Vector2f>{
		{100.f, 100.f},
		{1500.f, 100.f},
		{100.f, 800.f},
		{1500.f, 800.f}
	};

	for (int i = 0; i < Parameters::MAX_BOTS; ++i) {
		// wylosuj spawnpointa
		sf::Vector2f spawnPoint = spawnPoints[i];

		// stwórz bota i ustaw mu graf, węzęł, kolor i wstaw do listy botów
		auto bot = std::make_unique<Bot>(i, spawnPoint);
		bot->game = this;
		bot->setNavGraph(navGraph.get());
		bot->rocketLauncher.setMap(map.get());
		bot->selectRandomNode();
		bot->setColor(botColors[i]);
		bots.push_back(std::move(bot));
	}
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

void Game::spawnPickups() {
	// ustalone pozycje pickupów na mapie
	// health packi (czerwony)
	pickups.push_back(std::make_unique<HealthPack>(sf::Vector2f(300.f, 200.f)));
	pickups.push_back(std::make_unique<HealthPack>(sf::Vector2f(1140.f, 700.f)));
	pickups.push_back(std::make_unique<HealthPack>(sf::Vector2f(150.f, 670.f)));
	pickups.push_back(std::make_unique<HealthPack>(sf::Vector2f(1300.f, 350.f)));

	// railgun ammo (cyan)
	pickups.push_back(std::make_unique<RailGunAmmoPack>(sf::Vector2f(150.f, 425.f)));
	pickups.push_back(std::make_unique<RailGunAmmoPack>(sf::Vector2f(1450.f, 500.f)));
	pickups.push_back(std::make_unique<RailGunAmmoPack>(sf::Vector2f(800.f, 100.f)));
	pickups.push_back(std::make_unique<RailGunAmmoPack>(sf::Vector2f(800.f, 820.f)));

	// rocket ammo (zolty lol)
	pickups.push_back(std::make_unique<RocketAmmoPack>(sf::Vector2f(700.f, 350.f)));
	pickups.push_back(std::make_unique<RocketAmmoPack>(sf::Vector2f(1150.f, 200.f)));
	pickups.push_back(std::make_unique<RocketAmmoPack>(sf::Vector2f(450.f, 670.f)));
	pickups.push_back(std::make_unique<RocketAmmoPack>(sf::Vector2f(1140.f, 650.f)));
}

void Game::updatePickups(float deltaTime) {
	for (auto& pickup : pickups) {
		pickup->update(deltaTime);

		// sprawdź kolizje z botami
		for (auto& bot : bots) {
			if (pickup->checkCollision(bot->getPosition(), Parameters::BOT_RADIUS)) {
				pickup->applyEffect(bot.get());
			}
		}
	}
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