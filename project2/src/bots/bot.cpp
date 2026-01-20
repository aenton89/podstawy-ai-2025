#include "bot.h"
#include "../map/navigation_graph.h"
#include "../game/game.h"
#include <cmath>

Bot::Bot(int id, const sf::Vector2f& startPosition) : id(id), position(startPosition) {}

void Bot::selectRandomNode() {
    if (!navGraph) return;

    const auto& nodes = navGraph->getNodes();
    if (nodes.empty()) return;

    // wylosuj węzęł
    std::uniform_int_distribution<size_t> dist(0, nodes.size() - 1);
    size_t nodeId = dist(Game::gen);
    sf::Vector2f targetPos = nodes[nodeId].position;
    // znajdź ścieżkę Doniego
    currentPath = navGraph->findPath(position, targetPos);
    currentPathIndex = 0;
}

void Bot::followPath(float deltaTime) {
    if (currentPath.empty() || currentPathIndex >= currentPath.size()) return;

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