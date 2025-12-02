#include "raycast.h"



std::optional<HitInfo> rayCircleIntersect(const sf::Vector2f& origin, const sf::Vector2f& dir, const Collider& circle) {
	sf::Vector2f m = origin - sf::Vector2f(circle.position.x, circle.position.y);
	float b = m.x * dir.x + m.y * dir.y;
	float c = m.x * m.x + m.y * m.y - circle.radius * circle.radius;

	float disc = b*b - c;
	if (disc < 0)
		return std::nullopt;

	float t = -b - std::sqrt(disc);
	if (t < 0)
		t = -b + std::sqrt(disc);
	if (t < 0)
		return std::nullopt;

	return HitInfo{origin + dir * t, t};
}



Raycast::Raycast(): line(sf::Lines, 2), active(false) {}

void Raycast::shoot(const sf::Vector2f& origin, const sf::Vector2f& direction, const std::vector<std::unique_ptr<Obstacle>>& obstacles, const std::vector<std::unique_ptr<Enemy>>& enemies) {
	// domyślnie bardzo daleko
	sf::Vector2f endPoint = origin + direction * 10000.f;
	float minDist = 10000.f;

	// ale może być kolizja z przeszkodami - wtedy ucinamy po prostu linie
	for (const auto& o : obstacles) {
		auto hit = rayCircleIntersect(origin, direction, o->collider);
		if (hit && hit->distance < minDist) {
			minDist = hit->distance;
			endPoint = hit->point;
		}
	}

	// albo z przeciwnikami - wtedy też ucinamy linię + usuwwamy przeciwnika
	for (auto& e : enemies) {
		auto hit = rayCircleIntersect(origin, direction, e->collider);
		if (hit && hit->distance < minDist) {
			// oznaczamy przeciwnika jako trafionego
			e->was_hit = true;

			minDist = hit->distance;
			endPoint = hit->point;
		}
	}

	line[0].position = origin;
	line[0].color = sf::Color::Cyan;
	line[1].position = endPoint;
	line[1].color = sf::Color::Cyan;
	active = true;
}

void Raycast::draw(sf::RenderWindow& window) const {
	if (active)
		window.draw(line);
}

void Raycast::clear() {
	active = false;
}

bool Raycast::isActive() const {
	return active;
}
