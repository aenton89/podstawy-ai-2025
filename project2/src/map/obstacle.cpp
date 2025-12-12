#include "obstacle.h"
#include <cmath>
#include <algorithm>



Obstacle::Obstacle(const std::vector<sf::Vector2f>& verts) : vertices(verts) {
    shape.setPointCount(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i) {
        shape.setPoint(i, vertices[i]);
    }

    shape.setFillColor(sf::Color(100, 100, 100));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);
}

bool Obstacle::pointInPolygon(const sf::Vector2f& point) const {
    int intersections = 0;
    size_t n = vertices.size();

    for (size_t i = 0; i < n; ++i) {
        sf::Vector2f v1 = vertices[i];
        sf::Vector2f v2 = vertices[(i + 1) % n];

        if ((v1.y > point.y) != (v2.y > point.y)) {
            float intersectX = (v2.x - v1.x) * (point.y - v1.y) / (v2.y - v1.y) + v1.x;
            if (point.x < intersectX)
                intersections++;
        }
    }

    return (intersections % 2) == 1;
}

float Obstacle::distanceToSegment(const sf::Vector2f& point, const sf::Vector2f& a, const sf::Vector2f& b) const {
    sf::Vector2f ab = b - a;
    sf::Vector2f ap = point - a;

    float abLen2 = ab.x * ab.x + ab.y * ab.y;
    if (abLen2 == 0.f) {
        sf::Vector2f diff = point - a;
        return std::sqrt(diff.x * diff.x + diff.y * diff.y);
    }

    float t = std::max(0.f, std::min(1.f, (ap.x * ab.x + ap.y * ab.y) / abLen2));
    sf::Vector2f projection = a + t * ab;
    sf::Vector2f diff = point - projection;

    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

bool Obstacle::containsPoint(const sf::Vector2f& point) const {
    return pointInPolygon(point);
}

bool Obstacle::intersectsCircle(const sf::Vector2f& center, float radius) const {
    // jeśli środek jest wewnątrz wielokąta, to jest kolizja
    if (pointInPolygon(center))
        return true;

    // sprawdź odległość do każdej krawędzi
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
        float dist = distanceToSegment(center, vertices[i], vertices[(i + 1) % n]);
        if (dist < radius)
            return true;
    }

    return false;
}

void Obstacle::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}