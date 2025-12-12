#include "navigation_graph.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>



NavigationGraph::NavigationGraph(Map* m) : map(m) {}

void NavigationGraph::generateGraph(const sf::Vector2f& startPoint) {
    nodes.clear();
    positionToIndex.clear();

    std::unordered_set<sf::Vector2f, Vector2fHash, Vector2fEqual> visited;

    // sprawdź czy punkt startowy jest poprawny
    if (!map->isPositionValid(startPoint, map->getBotRadius())) {
        std::cerr << "Start point is not valid for graph generation!" << std::endl;
        return;
    }

    std::cout << "Starting flood fill from (" << startPoint.x << ", " << startPoint.y << ")" << std::endl;
    floodFill(startPoint, visited);
    std::cout << "Graph generated with " << nodes.size() << " nodes" << std::endl;
}

void NavigationGraph::floodFill(const sf::Vector2f& position, std::unordered_set<sf::Vector2f, Vector2fHash, Vector2fEqual>& visited) {

    std::queue<sf::Vector2f> toVisit;
    toVisit.push(position);
    visited.insert(position);

    // krok jest równy średnicy bota
    float stepSize = map->getBotRadius() * 2.0f;

    while (!toVisit.empty()) {
        sf::Vector2f current = toVisit.front();
        toVisit.pop();

        // dodaj bieżącą pozycję jako węzeł
        int currentIdx = nodes.size();
        nodes.emplace_back(current);
        positionToIndex[current] = currentIdx;

        // sprawdza wszystkie 8 kierunków
        for (const auto& dir : directions) {
            sf::Vector2f newPos = current + dir * stepSize;

            // czy pozycja jest poprawna i czy nie była odwiedzona
            if (!visited.contains(newPos) && map->isPositionValid(newPos, map->getBotRadius())) {
                // dodatkowo - czy ścieżka do nowego punktu jest czysta
                if (map->isPathClear(current, newPos, map->getBotRadius())) {
                    visited.insert(newPos);
                    toVisit.push(newPos);
                }
            }
        }
    }

    // drugi raz - połącz węzły (dodaj krawędzie)
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (const auto& dir : directions) {
            sf::Vector2f neighborPos = nodes[i].position + dir * stepSize;

            auto it = positionToIndex.find(neighborPos);
            if (it != positionToIndex.end()) {
                int neighborIdx = it->second;

                // czy połączenie już nie istnieje
                if (std::ranges::find(nodes[i].neighbors, neighborIdx) == nodes[i].neighbors.end())
                    nodes[i].neighbors.push_back(neighborIdx);
            }
        }
    }
}

int NavigationGraph::findNearestNode(const sf::Vector2f& position) const {
    if (nodes.empty())
        return -1;

    int nearestIdx = 0;
    float minDist = std::numeric_limits<float>::max();

    for (size_t i = 0; i < nodes.size(); ++i) {
        sf::Vector2f diff = nodes[i].position - position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist < minDist) {
            minDist = dist;
            nearestIdx = i;
        }
    }

    return nearestIdx;
}

float NavigationGraph::heuristic(const sf::Vector2f& a, const sf::Vector2f& b) const {
    // odległość euklidesowa
    sf::Vector2f diff = b - a;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

std::vector<sf::Vector2f> NavigationGraph::findPath(int startIdx, int endIdx) const {
    std::vector<sf::Vector2f> path;

    if (startIdx < 0 || startIdx >= static_cast<int>(nodes.size()) || endIdx < 0 || endIdx >= static_cast<int>(nodes.size()))
        return path;

    // A*
    struct AStarNode {
        int idx;
        // koszt od startu
        float g;
        // g + heurystyka
        float f;
        int parent;

        bool operator>(const AStarNode& other) const {
            return f > other.f;
        }
    };

    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    std::unordered_set<int> closedSet;
    std::unordered_map<int, int> parentMap;
    std::unordered_map<int, float> gScore;

    // inicjalizacja
    for (size_t i = 0; i < nodes.size(); ++i) {
        gScore[i] = std::numeric_limits<float>::max();
    }
    gScore[startIdx] = 0.f;

    AStarNode start;
    start.idx = startIdx;
    start.g = 0.f;
    start.f = heuristic(nodes[startIdx].position, nodes[endIdx].position);
    start.parent = -1;

    openSet.push(start);

    while (!openSet.empty()) {
        AStarNode current = openSet.top();
        openSet.pop();

        if (current.idx == endIdx) {
            // odtwarza ścieżkę
            int idx = endIdx;
            while (idx != -1) {
                path.push_back(nodes[idx].position);
                idx = parentMap.contains(idx) ? parentMap[idx] : -1;
            }
            std::ranges::reverse(path);
            return path;
        }

        if (closedSet.contains(current.idx))
            continue;
        closedSet.insert(current.idx);

        // sprawdź sąsiadów
        for (int neighborIdx : nodes[current.idx].neighbors) {
            if (closedSet.contains(neighborIdx))
                continue;

            sf::Vector2f diff = nodes[neighborIdx].position - nodes[current.idx].position;
            float edgeCost = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            float tentativeG = current.g + edgeCost;

            if (tentativeG < gScore[neighborIdx]) {
                gScore[neighborIdx] = tentativeG;
                parentMap[neighborIdx] = current.idx;

                AStarNode neighbor;
                neighbor.idx = neighborIdx;
                neighbor.g = tentativeG;
                neighbor.f = tentativeG + heuristic(nodes[neighborIdx].position, nodes[endIdx].position);
                neighbor.parent = current.idx;

                openSet.push(neighbor);
            }
        }
    }

    // jeśli nie znaleziono zwraca pustą ścieżkę
    return path;
}

std::vector<sf::Vector2f> NavigationGraph::findPath(const sf::Vector2f& start, const sf::Vector2f& end) const {
    int startIdx = findNearestNode(start);
    int endIdx = findNearestNode(end);

    return findPath(startIdx, endIdx);
}

void NavigationGraph::draw(sf::RenderWindow& window, bool drawNodes, bool drawEdges) const {
    // rysuje krawędzie
    if (drawEdges) {
        sf::VertexArray lines(sf::Lines);

        for (size_t i = 0; i < nodes.size(); ++i) {
            for (int neighborIdx : nodes[i].neighbors) {
                lines.append(sf::Vertex(nodes[i].position, sf::Color(0, 255, 0, 100)));
                lines.append(sf::Vertex(nodes[neighborIdx].position, sf::Color(0, 255, 0, 100)));
            }
        }

        window.draw(lines);
    }

    // rysuje węzły
    if (drawNodes) {
        sf::CircleShape nodeShape(3.f);
        nodeShape.setFillColor(sf::Color::Yellow);
        nodeShape.setOrigin(3.f, 3.f);

        for (const auto& node : nodes) {
            nodeShape.setPosition(node.position);
            window.draw(nodeShape);
        }
    }
}