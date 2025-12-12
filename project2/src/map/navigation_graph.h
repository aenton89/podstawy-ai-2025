#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "map.h"



// struktura wierzchołka grafu
struct Node {
    sf::Vector2f position;
    // indeksy sąsiednich wierzchołków
    std::vector<int> neighbors;

    Node(const sf::Vector2f& pos) : position(pos) {}
};

// hash dla sf::Vector2f do użycia w unordered_map
struct Vector2fHash {
    std::size_t operator()(const sf::Vector2f& v) const {
        // zaokrągla do najbliższego integera, bo problemy z float precision
        int x = static_cast<int>(std::round(v.x));
        int y = static_cast<int>(std::round(v.y));
        return std::hash<int>()(x) ^ (std::hash<int>()(y) << 1);
    }
};

struct Vector2fEqual {
    bool operator()(const sf::Vector2f& a, const sf::Vector2f& b) const {
        return std::abs(a.x - b.x) < 0.5f && std::abs(a.y - b.y) < 0.5f;
    }
};



// klasa reprezentująca graf do nawigacji
class NavigationGraph {
public:
    NavigationGraph(Map* map);

    // generuje graf używając flood fill
    void generateGraph(const sf::Vector2f& startPoint);

    // znajduje najbliższy węzeł do danej pozycji
    int findNearestNode(const sf::Vector2f& position) const;

    // A* - znajduje najkrótszą ścieżkę między dwoma węzłami
    std::vector<sf::Vector2f> findPath(int startNodeIdx, int endNodeIdx) const;
    std::vector<sf::Vector2f> findPath(const sf::Vector2f& start, const sf::Vector2f& end) const;

    void draw(sf::RenderWindow& window, bool drawNodes = true, bool drawEdges = true) const;

    const std::vector<Node>& getNodes() const { return nodes; }

private:
    Map* map;
    std::vector<Node> nodes;

    // pomocnicze struktury dla flood fill
    std::unordered_map<sf::Vector2f, int, Vector2fHash, Vector2fEqual> positionToIndex;

    // 8 kierunków (góra, dół, lewo, prawo + przekątne)
    const std::vector<sf::Vector2f> directions = {
        // główne kierunki
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        // przekątne
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    void floodFill(const sf::Vector2f& position, std::unordered_set<sf::Vector2f, Vector2fHash, Vector2fEqual>& visited);
    float heuristic(const sf::Vector2f& a, const sf::Vector2f& b) const;
};