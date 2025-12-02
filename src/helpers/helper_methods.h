#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <random>



// tu sa deklaracje i definicje funkcji pomocniczych - inline rozwiązuje problem wielokrotnego include'pwania

// funkcja pomocnicza do obliczenia odległości między wektorami
inline float distanceVec2D(sf::Vector2f from, sf::Vector2f to) {
	float dx = from.x - to.x;
	float dy = from.y - to.y;
	return std::sqrt(dx * dx + dy * dy);
}

// funkcja pomocnicza do normalizacji wektora 2D
inline sf::Vector2f normalizeVec2D(const sf::Vector2f& v) {
	float len = std::sqrt(v.x * v.x + v.y * v.y);
	if (len != 0)
		return v / len;
	return {0.f, 0.f};
}

// przekształca punkt do lokalnej przestrzeni współrzędnych
inline sf::Vector2f pointToLocalSpace(const sf::Vector2f& point, const sf::Vector2f& heading, const sf::Vector2f& side, const sf::Vector2f& agentPos) {
	sf::Vector2f transPoint = point - agentPos;

	float localX = transPoint.x * heading.x + transPoint.y * heading.y;
	float localY = transPoint.x * side.x + transPoint.y * side.y;

	return {localX, localY};
}

inline sf::Vector2f vectorToWorldSpace(const sf::Vector2f& vec, const sf::Vector2f& heading, const sf::Vector2f& side) {
	return {
	heading.x * vec.x + side.x * vec.y,
	heading.y * vec.x + side.y * vec.y
	};
}

// f.pom. konwersja wektora lokalnego na globalny
inline sf::Vector2f pointToWorldSpace(const sf::Vector2f& local, const sf::Vector2f& heading, const sf::Vector2f& side, const sf::Vector2f& pos) {
	sf::Vector2f world;
	world.x = local.x * heading.x + local.y * side.x;
	world.y = local.x * heading.y + local.y * side.y;
	world += pos;
	return world;
}

// f.pom. losowania wartości między -1 i 1
inline double randomClamped() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<double> dist(-1.0, 1.0);
	return dist(gen);
}