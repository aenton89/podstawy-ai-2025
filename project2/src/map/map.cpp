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
    // left top corner
    addObstacle({
        {350.f, 150.f},
        {400.f, 150.f},
        {400.f, 300.f},
        {350.f, 300.f}
    });
    addObstacle({
        {150.f, 250.f},
        {350.f, 250.f},
        {350.f, 300.f},
        {150.f, 300.f}
    });

    // middle blob
    addObstacle({
        {750.f, 250.f},
        {850.f, 250.f},
        {1000.f, 400.f},
        {1000.f, 600.f},
        {900.f, 600.f},
        {750.f, 500.f}
    });
    addObstacle({
        {600.f, 400.f},
        {750.f, 400.f},
        {750.f, 500.f},
        {600.f, 500.f}
    });

    // below blob
    addObstacle({
        {750.f, 620.f},
        {900.f, 720.f},
        {900.f, 730.f},
        {750.f, 730.f}
    });
    addObstacle({
        {550.f, 620.f},
        {650.f, 620.f},
        {650.f, 730.f},
        {550.f, 730.f}
    });

    // top leftish square
    addObstacle({
        {550.f, 200.f},
        {650.f, 200.f},
        {650.f, 300.f},
        {550.f, 300.f}
    });

    // slots left side
    addObstacle({
        {250.f, 390.f},
        {350.f, 390.f},
        {350.f, 440.f},
        {250.f, 440.f}
    });
    addObstacle({
        {250.f, 500.f},
        {350.f, 500.f},
        {350.f, 550.f},
        {250.f, 550.f}
    });
    addObstacle({
        {250.f, 600.f},
        {350.f, 600.f},
        {350.f, 650.f},
        {250.f, 650.f}
    });
    addObstacle({
        {250.f, 700.f},
        {350.f, 700.f},
        {350.f, 750.f},
        {250.f, 750.f}
    });
    addObstacle({
        {250.f, 700.f},
        {350.f, 700.f},
        {350.f, 750.f},
        {250.f, 750.f}
    });
    addObstacle({
        {250.f, 810.f},
        {350.f, 810.f},
        {350.f, 860.f},
        {250.f, 860.f}
    });

    // right traingle fan
    addObstacle({
        {1150.f, 300.f},
        {1250.f, 200.f},
        {1250.f, 300.f}
    });
    addObstacle({
        {1350.f, 300.f},
        {1350.f, 200.f},
        {1450.f, 300.f}
    });
    addObstacle({
        {1150.f, 400.f},
        {1250.f, 400.f},
        {1250.f, 500.f}
    });
    addObstacle({
        {1350.f, 500.f},
        {1350.f, 400.f},
        {1450.f, 400.f}
    });

    // right bottom corner
    addObstacle({
        {1200.f, 600.f},
        {1450.f, 600.f},
        {1450.f, 750.f},
        {1200.f, 750.f}
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

bool Map::isCircleCollidingWithObstacles(const sf::Vector2f& pos, float radius) const {
    // sprawdź kolizję z przeszkodami
    for (const auto& obstacle : obstacles) {
        if (obstacle.intersectsCircle(pos, radius))
            return true;
    }

    return false;
}

void Map::draw(sf::RenderWindow& window) const {
    window.draw(border);

    for (const auto& obstacle : obstacles) {
        obstacle.draw(window);
    }
}