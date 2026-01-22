#include "bot.h"
#include "../map/navigation_graph.h"
#include "../game/game.h"
#include <cmath>
#include <iostream>

Bot::Bot(int id, const sf::Vector2f& startPosition) : id(id), position(startPosition) {}

void Bot::selectRandomNode() {
    if (!navGraph)
        return;

    const auto& nodes = navGraph->getNodes();
    if (nodes.empty())
        return;

    // wylosuj węzęł
    std::uniform_int_distribution<size_t> dist(0, nodes.size() - 1);
    size_t nodeId = dist(Game::gen);
    sf::Vector2f targetPos = nodes[nodeId].position;
    // znajdź ścieżkę do niego
    currentPath = navGraph->findPath(position, targetPos);
    currentPathIndex = 0;
}

void Bot::followPath(float deltaTime) {
    if (currentPath.empty() || currentPathIndex >= currentPath.size())
        return;

    sf::Vector2f targetWaypoint = currentPath[currentPathIndex];
    sf::Vector2f direction = targetWaypoint - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 5.0f) {
        direction /= distance;
        position += direction * Parameters::BOT_SPEED * deltaTime;
    }
    else {
        currentPathIndex++;
    }
}

bool Bot::hasArrived() const {
    return currentPathIndex >= currentPath.size();
}

// pod debug
void Bot::playerControl() {
    if (!navGraph)
        return;

    // samoobrażenia pod klawiszem 1, strzał pod 2 i 3
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
            health.takeDamage(10);
            std::cout << "Self damage! HP: " << health.getHealth() << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        railGun.shoot();
        std::cout << "RailGun shot! Ammo: " << railGun.getAmmo() << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        rocketLauncher.shoot();
        std::cout << "Rocket shot! Ammo: " << rocketLauncher.getAmmo() << std::endl;
    }

    const auto& nodes = navGraph->getNodes();
    if (nodes.empty())
        return;

    // znajdź najbliższy węzeł do aktualnej pozycji
    size_t currentNode = 0;
    float minDist = INFINITY;
    for (size_t i = 0; i < nodes.size(); ++i) {
        sf::Vector2f diff = nodes[i].position - position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < minDist) {
            minDist = dist;
            currentNode = i;
        }
    }

    // określ kierunek ruchu
    sf::Vector2f targetDirection(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        targetDirection.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        targetDirection.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        targetDirection.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        targetDirection.x += 1.f;

    // normalizacja
    float length = std::sqrt(targetDirection.x * targetDirection.x + targetDirection.y * targetDirection.y);
    if (length == 0.f) return;
    targetDirection /= length;

    // znajdź sąsiedni węzeł najbardziej zgodny z kierunkiem
    const auto& neighbors = nodes[currentNode].neighbors;
    if (neighbors.empty())
        return;

    size_t bestNeighbor = neighbors[0];
    float bestDot = -INFINITY;

    for (size_t neighborId : neighbors) {
        sf::Vector2f toNeighbor = nodes[neighborId].position - nodes[currentNode].position;
        float dist = std::sqrt(toNeighbor.x * toNeighbor.x + toNeighbor.y * toNeighbor.y);
        if (dist > 0.f) {
            toNeighbor /= dist;
            float dot = toNeighbor.x * targetDirection.x + toNeighbor.y * targetDirection.y;
            if (dot > bestDot) {
                bestDot = dot;
                bestNeighbor = neighborId;
            }
        }
    }

    // ustaw ścieżkę do wybranego sąsiada - tylko jeśli kierunek jest zgodny
    if (bestDot > 0.f) {
        currentPath = navGraph->findPath(position, nodes[bestNeighbor].position);
        currentPathIndex = 0;
    }
}