#pragma once
#include <cmath>
#include <memory>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include "../../helpers/helper_methods.h"

class GameObject;
class Enemy;
class Obstacle;




// enum dla zwalniania przy arrive
enum class ArriveDeceleration {
	slow = 3,
	normal = 2,
	fast = 1
};

// funkcja pomocnicza do tagowania sąsiadów w promieniu
template<class T, class conT>
void TagNeighbors(const T* entity, conT& containerOfEntities, double radius) {
	// pętla przez wszystkie encje sprawdzając zasięg
	for (auto& curEntity : containerOfEntities) {
		// najpierw wyczyść obecny tag
		curEntity->unTag();

		sf::Vector2f to = curEntity->getPosition() - entity->getPosition();

		// promień ograniczający drugiej encji jest brany pod uwagę przez dodanie go do zasięgu
		double range = radius + curEntity->collider.radius;

		// jeśli encja w zasięgu, dajemy jej tag do dalszej analizy
		float lengthSq = to.x * to.x + to.y * to.y;
		if ((curEntity.get() != entity) && (lengthSq < range * range))
			curEntity->tag();
	}
}



// klasa odpowiadająca za obliczanie steering behaviours
class SteeringBehaviours {
public:
	Enemy* parent;
	// potrzebne pod wall avoidance
	std::vector<sf::Vector2f> feelers;
	sf::Vector2f wanderTarget;



	SteeringBehaviours();

	void setParent(Enemy* _parent);
	// funkcja tworząca feelery do wall avoidance
	void createFeelers();
	// znajduje najbliższy obiekt na kolizji i zwraca wyniki przez referencje
	void findIntersections(const std::vector<GameObject*>& objects, GameObject*& closestObject, double& distToClosestIP, sf::Vector2f& localPosOfClosest, float& dBoxLength, sf::Vector2f& heading, sf::Vector2f& side);
	// DEBUG: wizualizacja detection box i przecięć do obstacle avoidance
	void debugDrawIntersections(sf::RenderWindow& window, const sf::Vector2f& heading, float dBoxLength, GameObject* closestObstacle, double distToClosestIP);
	// pomocnicza funkcja do hide - oblicza pozycję ukrycia za przeszkodą
	sf::Vector2f getHidingPosition(const sf::Vector2f& posOb, float radiusOb, const sf::Vector2f& posTarget);

	// obliczaja kolejne siły wpływające na Enemy
	sf::Vector2f seek(const sf::Vector2f& targetPos);
	sf::Vector2f wander();
	sf::Vector2f hide(const std::vector<std::unique_ptr<Obstacle>>& obstacles);
	sf::Vector2f flee(const sf::Vector2f& targetPos);
	sf::Vector2f evade();
	sf::Vector2f arrive(const sf::Vector2f& targetPos, ArriveDeceleration deceleration);
	// zachowania grupowe
	sf::Vector2f separation(const std::vector<std::unique_ptr<Enemy>>& neighbors);
	sf::Vector2f alignment(const std::vector<std::unique_ptr<Enemy>>& neighbors);
	sf::Vector2f cohesion(const std::vector<std::unique_ptr<Enemy>>& neighbors);
	// unikanie przeszkód
	sf::Vector2f wallAvoidance();
	sf::Vector2f obstacleAvoidance(const std::vector<std::unique_ptr<Obstacle>>& obstacles, const std::vector<std::unique_ptr<Enemy>>& enemies);
	// oblicza sume z wszystkich uwzględnianych wektorów
	sf::Vector2f calculate();
	// funkcje z wybranymi steering behaviours dla konkretnych stanów
	sf::Vector2f attack();
	sf::Vector2f hide_explore();

	bool accumulateForce(sf::Vector2f& runningTot, sf::Vector2f forceToAdd);
};
