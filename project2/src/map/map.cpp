#include "map.h"



Map::Map(float w, float h, float radius) : width(w), height(h), botRadius(radius) {

    border.setSize(sf::Vector2f(width, height));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::Red);
    border.setOutlineThickness(3.f);

    generateDefaultObstacles();
}

void Map::addObstacle(const std::vector<sf::Vector2f>& vertices) {
    obstacles.emplace_back(vertices);
}

// tu jakieś losowe przeszkody na mapie
void Map::generateDefaultObstacles() {
    addObstacle({
        {150.f, 150.f},
        {400.f, 150.f},
        {400.f, 300.f},
        {150.f, 300.f}
    });

    addObstacle({
        {1200.f, 600.f},
        {1450.f, 600.f},
        {1450.f, 750.f},
        {1200.f, 750.f}
    });

    addObstacle({
        {750.f, 300.f},
        {850.f, 300.f},
        {850.f, 600.f},
        {750.f, 600.f}
    });

    addObstacle({
        {600.f, 400.f},
        {1000.f, 400.f},
        {1000.f, 500.f},
        {600.f, 500.f}
    });
}

bool Map::isPositionValid(const sf::Vector2f& pos, float radius) const {
    // sprawdź czy bot nie wychodzi poza mapę
    if (pos.x - radius < 0 || pos.x + radius > width ||
        pos.y - radius < 0 || pos.y + radius > height) {
        return false;
    }

    // sprawdź kolizję z przeszkodami
    for (const auto& obstacle : obstacles) {
        if (obstacle.intersectsCircle(pos, radius))
            return false;
    }

    return true;
}

bool Map::isPathClear(const sf::Vector2f& from, const sf::Vector2f& to, float radius) const {
    // prosta interpolacja - sprawdza punkty wzdłuż linii
    float distance = std::sqrt(std::pow(to.x - from.x, 2) + std::pow(to.y - from.y, 2));
    int steps = static_cast<int>(distance / (radius * 0.5f)) + 1;

    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        sf::Vector2f pos = from + t * (to - from);

        if (!isPositionValid(pos, radius))
            return false;
    }

    return true;
}

void Map::draw(sf::RenderWindow& window) const {
    window.draw(border);

    for (const auto& obstacle : obstacles) {
        obstacle.draw(window);
    }
}