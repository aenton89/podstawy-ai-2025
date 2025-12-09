#include "game.h"
#include "../helpers/parameters.h"
#include <iostream>
#include <SFML/Graphics.hpp>



// zdefiniowanie statycznych w .cpp
std::mt19937 Game::gen{ std::random_device{}() };

Game::Game(): window(sf::VideoMode(1600, 900), "ASSIGNMENT 1: zombie game"), player(PLAYER_SPEED, this) {
	sf::Vector2u winSize = window.getSize();
	player.setPosition({ winSize.x / 2.f, winSize.y / 2.f });

	initObstacles(MAX_OBSTACLES_AMOUNT);
	spawnEnemies(MAX_ENEMIES_AMOUNT);
}

void Game::initObstacles(int count) {
	sf::Vector2u winSize = window.getSize();

	for (int i = 0; i < count; ++i) {
		bool placed = false;
		int attempts = 0;

		// max 10 prób na każdą przeszkodę
		while (!placed && attempts < 10) {
			// generowanie losowej pozycji i rozmiaru
			std::uniform_real_distribution<float> distX(0.f, static_cast<float>(winSize.x));
			std::uniform_real_distribution<float> distY(0.f, static_cast<float>(winSize.y));
			std::uniform_real_distribution<float> distR(MIN_OBSTACLE_SIZE, MAX_OBSTACLE_SIZE);
			auto x = distX(gen);
			auto y = distY(gen);
			float r = distR(gen);

			Obstacle newObstacle(x, y, r);

			// sprawdzenie kolizji z graczem i innymi przeszkodami
			if (player.collider.checkCollision(newObstacle.collider)) {
				attempts++;
				continue;
			}

			bool overlap = false;
			for (const auto& existing : obstacles) {
				if (existing->collider.checkCollision(newObstacle.collider)) {
					overlap = true;
					break;
				}
			}

			if (!overlap) {
				obstacles.push_back(std::make_unique<Obstacle>(newObstacle));
				placed = true;
			} else {
				attempts++;
			}
		}
	}
}

// wrogownie pojawiaja się losowo w zakresie MIN_ENEMY_EDGE_DIST do MAX_ENEMY_EDGE_DIST pikseli od krawędzi ekranu
void Game::spawnEnemies(int max_amount) {
    if (enemies.size() >= max_amount)
        return;

    sf::Vector2u winSize = window.getSize();

    while (enemies.size() < max_amount) {
    	std::uniform_int_distribution<int> distEdge(0, 3);
    	int edge = distEdge(gen);

        float x = 0.f, y = 0.f;

    	std::uniform_real_distribution<float> distOffset(MIN_ENEMY_EDGE_DIST,MAX_ENEMY_EDGE_DIST);

    	std::uniform_real_distribution<float> distScreenX(0.f, static_cast<float>(winSize.x));
    	std::uniform_real_distribution<float> distScreenY(0.f, static_cast<float>(winSize.y));

        switch (edge) {
            case 0:
        		x = distOffset(gen);
        		y = distScreenY(gen);
                break;
            case 1:
        		x = winSize.x - distOffset(gen);
        		y = distScreenY(gen);
                break;
            case 2:
        		x = distScreenX(gen);
        		y = distOffset(gen);
                break;
            case 3:
        		x = distScreenX(gen);
        		y = winSize.y - distOffset(gen);
                break;
        }

        // tymczasowy wróg do testu kolizji
        Enemy testEnemy(x, y, &player);

    	// sprawdzamy kolizje z przeszkodami i innymi wrogami i graczem
        bool overlap = player.collider.checkCollision(testEnemy.collider);

        if (!overlap) {
            for (const auto& obs : obstacles) {
                if (obs->collider.checkCollision(testEnemy.collider)) {
                    overlap = true;
                    break;
                }
            }
        }

        if (!overlap) {
            for (const auto& enemy : enemies) {
                if (enemy->collider.checkCollision(testEnemy.collider)) {
                    overlap = true;
                    break;
                }
            }
        }

    	// tworzymy wroga bezpośrednio w wektorze - wcześniej był bład bo yadayada zmiana na smart_pointers
        if (!overlap) {
            auto enemy = std::make_unique<Enemy>(x, y, &player);
            enemies.push_back(std::move(enemy));
        }
    }
}

void Game::deleteDeadEnemies() {
	std::erase_if(enemies, [](const std::unique_ptr<Enemy>& e) {
		return e->was_hit;
	});
}

// zmien stan wrogów na Attack
void Game::updateAgentsState() const {
    for (auto& enemy : enemies) {
        if (enemy->getState() == State::Attack)
        	continue;

		// lista wrogów w stanie HideExplore
        auto nearbyList = enemy->getNeighbours(enemies, GROUP_DISTANCE);
        if (static_cast<int>(nearbyList.size()) >= GROUP_THRESHOLD) {
            for (Enemy* e : nearbyList) {
	            if (e->getState() != State::Attack)
	            	e->setState(State::Attack);
            }

            enemy->setState(State::Attack);
        }
    }
}

void Game::keepInsideWindow(GameObject& obj) const {
	float r = obj.collider.radius;
	sf::Vector2f pos = obj.getPosition();

	// ograniczenie do granic okna
	if (pos.x - r < 0)
		pos.x = r;
	if (pos.x + r > window.getSize().x)
		pos.x = window.getSize().x - r;
	if (pos.y - r < 0)
		pos.y = r;
	if (pos.y + r > window.getSize().y)
		pos.y = window.getSize().y - r;

	obj.setPosition(pos);
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
	// TODO: DEBUG
	debug();

	// GRACZ: update + kolizje
	sf::Vector2f prevPos = player.getPosition();
	player.update(deltaTime, window);
	keepInsideWindow(player);

	for (auto& o : obstacles) {
		if (player.collider.checkCollision(o->collider)) {
			// cofnij pozycję
			player.setPosition(prevPos);
			player.collider.position = { prevPos.x, prevPos.y };
			break;
		}
	}

	// WROGOWIE: update + kolizje + spawnowanie
	for (auto& e : enemies) {
		e->update(deltaTime, window);
		keepInsideWindow(*e);

		// kolizje z przeszkodami
		for (auto& o : obstacles) {
			if (e->collider.checkCollision(o->collider)) {
				// cofnij pozycję
				e->setPosition(e->prevPosition);
				e->collider.position = { e->prevPosition.x, e->prevPosition.y };
				break;
			}
		}

		// kolizje z graczem
		if (e->collider.checkCollision(player.collider)) {
			gameOver();
		}

		// TODO: czy one mają być??? - bo jak tak, to trzeba uwzględnić wrogów w obstacle avoidance???
		// kolizje wrog-wrog
		for (auto& other : enemies) {
			// pomiń siebie
			if (other.get() == e.get())
				continue;

			if (e->collider.checkCollision(other->collider)) {
				// cofnij pozycję
				e->setPosition(e->prevPosition);
				e->collider.position = { e->prevPosition.x, e->prevPosition.y };
				break;
			}
		}
	}
	// usuń + dodaj przeciwników, jeśli gracz ich zabije
	deleteDeadEnemies();
	spawnEnemies(MAX_ENEMIES_AMOUNT);
	updateAgentsState();

	// RAYCAST
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		sf::Vector2f dir = player.getForwardDirection(window);
		raycast.shoot(player.getPosition(), dir, obstacles, enemies);
	} else {
		raycast.clear();
	}
}

void Game::render() {
	window.clear(sf::Color::Black);

	// rysuj przeszkody
	for (auto& o : obstacles)
		o->draw(window);

	// no i rysuj przeciwników
	for (auto& e : enemies)
		e->draw(window);

	// rysuj gracza i promień
	raycast.draw(window);
	player.draw(window);

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

void Game::debug() const {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
		std::cout<< "dt: " << clock.getElapsedTime().asSeconds() << "\n";
		std::cout<< "player pos: " << player.getPosition().x << ", " << player.getPosition().y << "\n";
		std::cout<< "random enemy -> player -> pos: " << enemies[0]->player->getPosition().x << ", " << enemies[0]->player->getPosition().y << "\n";
		std::cout<< "random steering behaviour -> parent -> player -> pos: " << enemies[0]->steering.parent->player->getPosition().x << ", " << enemies[0]->steering.parent->player->getPosition().y << "\n";
		std::cout<< "random enemy velocity: " << enemies[0]->velocity.x << ", " << enemies[0]->velocity.y << "\n";
		std::cout<< "random enemy pos: " << enemies[0]->getPosition().x << ", " << enemies[0]->getPosition().y << "\n";
		std::cout<< "random steering behaviour -> parent -> pos: " << enemies[0]->steering.parent->getPosition().x << ", " << enemies[0]->steering.parent->getPosition().y << "\n";
	}
}