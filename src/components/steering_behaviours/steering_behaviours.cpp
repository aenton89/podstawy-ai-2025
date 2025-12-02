#include "steering_behaviours.h"
#include "../../game/game.h"
#include "../../game_objects/enemy/enemy.h"
#include "../../game_objects/obstacle/obstacle.h"
#include "../../helpers/parameters.h"
#include "../../helpers/helper_methods.h"
#include <cmath>



SteeringBehaviours::SteeringBehaviours() {
    // startowa prędkość dla Wander
    wanderTarget = sf::Vector2f(WANDER_RADIUS, 0.f);
}

void SteeringBehaviours::setParent(Enemy *_parent) {
	parent = _parent;
}

sf::Vector2f SteeringBehaviours::calculate() {
	if (!parent)
		return {0, 0};

    // tegowanie pod zachowania grupowe
    tagNeighbors(parent, parent->player->game->enemies, NEIGHBOR_RADIUS);

    auto steeringForce = sf::Vector2f(0, 0);

    // -- alternatywna metoda "Weighted Truncated Running Sum with Prioritization" z prioretyzacją konkretnych steering forces --
    
    // sf::Vector2f force;
    // if (parent->getState() == State::Hide_Explore) {
    //     force = wallAvoidance() * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = obstacleAvoidance(parent->player->game->obstacles, parent->player->game->enemies) * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = flee(parent->player->getPosition()) * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = wander() * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = hide(parent->player->game->obstacles) * 2.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    // }
    // else {
    //     force = wallAvoidance() * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = obstacleAvoidance(parent->player->game->obstacles, parent->player->game->enemies) * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = separation(parent->player->game->enemies) * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = seek(parent->player->getPosition()) * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = alignment(parent->player->game->enemies) * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     force = cohesion(parent->player->game->enemies) * 1.0f;
    //     if (!accumulateForce(steeringForce, force)) return steeringForce;
    //     }
    // }
    // force = evade() * 0.0f;
    // if (!accumulateForce(steeringForce, force)) return steeringForce;
    //  
    // return steeringForce;

    if (parent->getState() == State::Hide_Explore)
        steeringForce = hide_explore();
    else
        steeringForce = attack();
    return steeringForce;
}

// zestaw sił wyliczanych w trybie Hide_Explore
sf::Vector2f SteeringBehaviours::hide_explore() {
    sf::Vector2f forceSum = sf::Vector2f(0, 0);
    forceSum += wallAvoidance() * MULT_WALL_AVOIDANCE;
    forceSum += obstacleAvoidance(parent->player->game->obstacles, parent->player->game->enemies) * MULT_OBSTACLE_AVOIDANCE;
    forceSum += wander() * MULT_WANDER;
    forceSum += hide(parent->player->game->obstacles) * MULT_HIDE;
    forceSum += flee(parent->player->getPosition()) * MULT_FLEE;
    forceSum += separation(parent->player->game->enemies) * MULT_SEPARATION;
    forceSum += alignment(parent->player->game->enemies) * MULT_ALIGNMENT;
    forceSum += cohesion(parent->player->game->enemies) * MULT_COHESION;
    // forceSum += arrive(parent->player->getPosition(), ArriveDeceleration::normal) * 1.0f;
    return forceSum;
}
// i trybie Attack
sf::Vector2f SteeringBehaviours::attack() {
    sf::Vector2f forceSum = sf::Vector2f(0, 0);
    forceSum += wallAvoidance() * MULT_WALL_AVOIDANCE;
    forceSum += obstacleAvoidance(parent->player->game->obstacles, parent->player->game->enemies) * MULT_OBSTACLE_AVOIDANCE;
    forceSum += seek(parent->player->getPosition()) * MULT_SEEK;
    return forceSum;
}

// pod kalkulację steering force z prioretyzacją
bool SteeringBehaviours::accumulateForce(sf::Vector2f& runningTot, sf::Vector2f forceToAdd) {
    double magnitudeSoFar = std::sqrt(runningTot.x * runningTot.x + runningTot.y * runningTot.y);
    double magnitudeRemaining = MAX_FORCE - magnitudeSoFar;
    if (magnitudeRemaining <= 0.0)
        return false;

    double magnitudeToAdd = std::sqrt(forceToAdd.x * forceToAdd.x + forceToAdd.y * forceToAdd.y);
    if (magnitudeToAdd < magnitudeRemaining)
        runningTot += forceToAdd;
    else
        runningTot += (normalizeVec2D(forceToAdd) * static_cast<float>(magnitudeRemaining));
    return true;
}

// --- seek ---

sf::Vector2f SteeringBehaviours::seek(const sf::Vector2f& targetPos) const {
	sf::Vector2f desired_vel = normalizeVec2D(targetPos - parent->getPosition()) * MAX_ENEMY_SPEED;
	return (desired_vel - parent->velocity);
}

// --- wander ---

sf::Vector2f SteeringBehaviours::wander() {
    wanderTarget += sf::Vector2f(randomClamped() * WANDER_JITTER, randomClamped() * WANDER_JITTER);
    wanderTarget = normalizeVec2D(wanderTarget);
    wanderTarget = sf::Vector2f(wanderTarget.x * WANDER_RADIUS, wanderTarget.y * WANDER_RADIUS);

    sf::Vector2f targetLocal = wanderTarget + sf::Vector2f(WANDER_DISTANCE, 0.f);
    sf::Vector2f targetWorld = pointToWorldSpace(targetLocal, parent->heading, parent->side, parent->getPosition());

    return targetWorld - parent->getPosition();
}

// --- wall avoidance ---

sf::Vector2f SteeringBehaviours::wallAvoidance() {
    createFeelers();
    sf::Vector2f steeringForce(0.f, 0.f);

    // Uproszczony, dla 4-ech ścian = brzegów ekranu
    for (const auto& feeler : feelers) {
        if (feeler.x < 0) {
            float overshoot = -feeler.x;
            steeringForce += sf::Vector2f(1.f, 0.f) * overshoot;
        }
        else if (feeler.x > 1600.f) {
            float overshoot = feeler.x - 1600.f;
            steeringForce += sf::Vector2f(-1.f, 0.f) * overshoot;
        }

        if (feeler.y < 0) {
            float overshoot = -feeler.y;
            steeringForce += sf::Vector2f(0.f, 1.f) * overshoot;
        }
        else if (feeler.y > 900.f) {
            float overshoot = feeler.y - 900.f;
            steeringForce += sf::Vector2f(0.f, -1.f) * overshoot;
        }
    }
    return steeringForce * 50.f;
}

void SteeringBehaviours::createFeelers(){
    feelers.clear();

    sf::Vector2f pos = parent->getPosition();
    sf::Vector2f heading = parent->heading;
    float feelerLength = 80.f;

    feelers.push_back(pos + heading * feelerLength);

    auto rotate = [](const sf::Vector2f& v, float degrees) {
        float rad = degrees * 3.1415926f / 180.0f;
        float cs = std::cos(rad);
        float sn = std::sin(rad);
        return sf::Vector2f(v.x * cs - v.y * sn, v.x * sn + v.y * cs);
    };
    sf::Vector2f leftFeeler  = pos + rotate(heading, 45.0f)  * feelerLength * 0.5f;
    sf::Vector2f rightFeeler = pos + rotate(heading, -45.0f) * feelerLength * 0.5f;

    feelers.push_back(leftFeeler);
    feelers.push_back(rightFeeler);
}

// --- obstacle avoidance ---

sf::Vector2f SteeringBehaviours::obstacleAvoidance(const std::vector<std::unique_ptr<Obstacle>>& obstacles, const std::vector<std::unique_ptr<Enemy>>& enemies) const {
    if (!parent)
        return {0, 0};

    // przygotuj wektor wszystkich obiektów do sprawdzenia
    std::vector<GameObject*> allObjects;

    // dodaj obstacles
    for (const auto& obstacle : obstacles) {
        allObjects.push_back(obstacle.get());
    }

    // dodaj enemies (pomijając siebie - findIntersections to też sprawdzi, ale można wcześniej)
    for (const auto& enemy : enemies) {
        allObjects.push_back(enemy.get());
    }

    // wywołaj findIntersections
    GameObject* closestObject = nullptr;
    double distToClosestIP = 0.0;
    sf::Vector2f localPosOfClosest;
    float dBoxLength = 0.0f;
    sf::Vector2f heading, side;

    findIntersections(allObjects, closestObject, distToClosestIP, localPosOfClosest, dBoxLength, heading, side);

    // jeśli znaleziono obiekt, oblicz siłę sterującą
    sf::Vector2f steeringForce(0, 0);

    if (closestObject) {
        // im bliżej obiektu, tym silniejsza siła
        float multiplier = 1.0f + (dBoxLength - distToClosestIP) / dBoxLength;

        // siła w osi bocznej (lateral force)
        steeringForce.y = (closestObject->collider.radius - localPosOfClosest.y) * multiplier;

        // siła hamująca
        const float brakingWeight = 0.2f;
        steeringForce.x = (closestObject->collider.radius - localPosOfClosest.x) * brakingWeight;

        // przekształć do world space
        steeringForce = vectorToWorldSpace(steeringForce, heading, side);
    }

    return steeringForce;
}

void SteeringBehaviours::findIntersections(const std::vector<GameObject*>& objects, GameObject*& closestObject, double& distToClosestIP, sf::Vector2f& localPosOfClosest, float& dBoxLength, sf::Vector2f& heading, sf::Vector2f& side) const {
    if (!parent)
        return;

    // długość detection box proporcjonalna do prędkości
    float speed = std::sqrt(parent->velocity.x * parent->velocity.x + parent->velocity.y * parent->velocity.y);
    dBoxLength = MIN_DETECTION_BOX_LENGTH + (speed / MAX_ENEMY_SPEED) * MIN_DETECTION_BOX_LENGTH;

    // zresetuj wyniki
    closestObject = nullptr;
    distToClosestIP = std::numeric_limits<double>::max();
    localPosOfClosest = sf::Vector2f(0, 0);

    // oblicz heading i side dla parent
    heading = normalizeVec2D(parent->velocity);
    side = sf::Vector2f(-heading.y, heading.x);

    for (auto* object : objects) {
        // pomiń siebie (jeśli object jest Enemy)
        if (object == parent)
            continue;

        float distToObject = distanceVec2D(parent->getPosition(), object->getPosition());

        if (distToObject < dBoxLength + object->collider.radius) {
            sf::Vector2f localPos = pointToLocalSpace(
                object->getPosition(),
                heading,
                side,
                parent->getPosition()
            );

            if (localPos.x >= 0) {
                float expandedRadius = object->collider.radius + parent->collider.radius;

                if (std::abs(localPos.y) < expandedRadius) {
                    double cX = localPos.x;
                    double cY = localPos.y;

                    double sqrtPart = std::sqrt(expandedRadius * expandedRadius - cY * cY);
                    double ip = cX - sqrtPart;

                    if (ip <= 0)
                        ip = cX + sqrtPart;

                    if (ip < distToClosestIP) {
                        distToClosestIP = ip;
                        closestObject = object;
                        localPosOfClosest = localPos;
                    }
                }
            }
        }
    }
}

// TODO: do debugowania dla obstacle avoidance
void SteeringBehaviours::debugDrawIntersections(sf::RenderWindow& window, const sf::Vector2f& heading, float dBoxLength, GameObject* closestObstacle, double distToClosestIP) const {
    if (!parent)
        return;

    // rysuj detection box
    sf::RectangleShape detectionBox;
    detectionBox.setSize(sf::Vector2f(dBoxLength, parent->collider.radius * 2));
    detectionBox.setOrigin(0, parent->collider.radius);
    detectionBox.setPosition(parent->getPosition());

    float angle = std::atan2(heading.y, heading.x) * 180.0f / 3.14159f;
    detectionBox.setRotation(angle);
    detectionBox.setFillColor(sf::Color(255, 255, 0, 50));
    detectionBox.setOutlineColor(sf::Color::Yellow);
    detectionBox.setOutlineThickness(1.0f);
    window.draw(detectionBox);

    // rysuj linię i markery do najbliższej przeszkody
    if (closestObstacle) {
        // linia do przeszkody
        sf::Vertex line[] = {
            sf::Vertex(parent->getPosition(), sf::Color::Red),
            sf::Vertex(closestObstacle->getPosition(), sf::Color::Red)
        };
        window.draw(line, 2, sf::Lines);

        // punkt przecięcia
        sf::Vector2f intersectionPoint = parent->getPosition() + heading * static_cast<float>(distToClosestIP);
        sf::CircleShape intersectionMarker(5.0f);
        intersectionMarker.setOrigin(5.0f, 5.0f);
        intersectionMarker.setPosition(intersectionPoint);
        intersectionMarker.setFillColor(sf::Color::Magenta);
        window.draw(intersectionMarker);

        // podświetlenie przeszkody
        sf::CircleShape highlight(closestObstacle->collider.radius);
        highlight.setOrigin(closestObstacle->collider.radius, closestObstacle->collider.radius);
        highlight.setPosition(closestObstacle->getPosition());
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineColor(sf::Color::Red);
        highlight.setOutlineThickness(2.0f);
        window.draw(highlight);
    }
}

// --- hide ---

// znajduje najlepszą przeszkodę do ukrycia się przed graczem
sf::Vector2f SteeringBehaviours::hide(const std::vector<std::unique_ptr<Obstacle>>& obstacles) const {
    if (!parent || !parent->player)
        return {0, 0};

    float distToClosest = std::numeric_limits<float>::max();
    sf::Vector2f bestHidingSpot;

    // przejdź przez wszystkie przeszkody i znajdź najbliższy punkt ukrycia
    for (const auto& obstacle : obstacles) {
        sf::Vector2f hidingSpot = getHidingPosition(
            obstacle->getPosition(),
            obstacle->collider.radius,
            parent->player->getPosition()
        );

        // oblicz odległość bota do tego punktu ukrycia
        float dist = distanceVec2D(parent->getPosition(), hidingSpot);

        // jeśli to najbliższy punkt ukrycia do tej pory, zapamiętaj go
        if (dist < distToClosest) {
            distToClosest = dist;
            bestHidingSpot = hidingSpot;
        }
    }

    // jeśli znaleziono punkt ukrycia, użyj arrive do dotarcia tam
    if (distToClosest != std::numeric_limits<float>::max())
        return arrive(bestHidingSpot, ArriveDeceleration::normal);

    // jeśli nie ma przeszkód, po prostu uciekaj od gracza (evade)
    return evade();
}

// funkcja pomocnicza - blicza pozycję ukrycia za przeszkodą
sf::Vector2f SteeringBehaviours::getHidingPosition(const sf::Vector2f& posOb, float radiusOb, const sf::Vector2f& posTarget) {
    // oblicz jak daleko od krawędzi przeszkody ma być bot
    const float DistanceFromBoundary = 30.0f;
    float distAway = radiusOb + DistanceFromBoundary;

    // oblicz kierunek od gracza (target) do przeszkody
    sf::Vector2f toOb = normalizeVec2D(posOb - posTarget);

    // przeskaluj do odpowiedniej odległości i dodaj do pozycji przeszkody
    return toOb * distAway + posOb;
}

// --- flee ---

// ucieka od zadanej pozycji + tylko w zasięgu panic distance
sf::Vector2f SteeringBehaviours::flee(const sf::Vector2f& targetPos) const {
    if (!parent)
        return {0, 0};

    float distSq = (parent->getPosition().x - targetPos.x) * (parent->getPosition().x - targetPos.x) +
                   (parent->getPosition().y - targetPos.y) * (parent->getPosition().y - targetPos.y);

    if (distSq > PANIC_DISTANCE_SQ)
        return {0, 0};

    sf::Vector2f desired_vel = normalizeVec2D(parent->getPosition() - targetPos) * MAX_ENEMY_SPEED;
    return (desired_vel - parent->velocity);
}

// --- evade ---

// ucieka od gracza przewidując jego przyszłą pozycję
sf::Vector2f SteeringBehaviours::evade() const {
    if (!parent || !parent->player)
        return {0, 0};

    sf::Vector2f toPursuer = parent->player->getPosition() - parent->getPosition();

    // czas wyprzedzenia jest proporcjonalny do odległości między graczem a botem i odwrotnie proporcjonalny do sumy ich prędkości
    float pursuerSpeed = std::sqrt(parent->player->velocity.x * parent->player->velocity.x + parent->player->velocity.y * parent->player->velocity.y);

    float toPursuerLength = std::sqrt(toPursuer.x * toPursuer.x + toPursuer.y * toPursuer.y);
    float lookAheadTime = toPursuerLength / (MAX_ENEMY_SPEED + pursuerSpeed);

    // uciekaj od przewidywanej przyszłej pozycji gracza
    sf::Vector2f predictedPos = parent->player->getPosition() + parent->player->velocity * lookAheadTime;
    return flee(predictedPos);
}

// --- arrive ---

// zmierza do celu i zwalnia w miarę zbliżania się
sf::Vector2f SteeringBehaviours::arrive(const sf::Vector2f& targetPos, ArriveDeceleration deceleration) const {
    if (!parent)
        return {0, 0};

    sf::Vector2f toTarget = targetPos - parent->getPosition();
    float dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

    if (dist > 0) {
        // prędkość potrzebną do osiągnięcia celu przy zadanym spowolnieniu; wartość z enum mnożona przez takie cosik, żeby dostosować do sensownych wartości
        float speed = dist / (static_cast<float>(deceleration) * DECELERATION_TWEAK);

        // prędkość nie przekracza max speed
        speed = std::min(speed, MAX_ENEMY_SPEED);

        // odtąd jak seek, ale bez normalizacji toTarget - bo mamy długość
        sf::Vector2f desiredVelocity = toTarget * speed / dist;

        return (desiredVelocity - parent->velocity);
    }

    return {0, 0};
}

// --- separation ---

// trzyma bota z dala od innych botów (group behaviour)
sf::Vector2f SteeringBehaviours::separation(const std::vector<std::unique_ptr<Enemy>>& neighbors) const {
    if (!parent)
        return {0, 0};

    sf::Vector2f steeringForce(0, 0);

    for (const auto& neighbor : neighbors) {
        // tylko otagowani sąsiedzi są brani pod uwagę
        if (neighbor->isTagged()) {
            sf::Vector2f toAgent = parent->getPosition() - neighbor->getPosition();
            float length = std::sqrt(toAgent.x * toAgent.x + toAgent.y * toAgent.y);

            if (length > 0)
                steeringForce += (toAgent / length) / length;
        }
    }

    return steeringForce;
}

// --- alignment ---

// wyrównuje kierunek bota do kierunków sąsiednich botów (group behaviour)
sf::Vector2f SteeringBehaviours::alignment(const std::vector<std::unique_ptr<Enemy>>& neighbors) const {
    if (!parent)
        return {0, 0};

    sf::Vector2f averageHeading(0, 0);
    int neighborCount = 0;

    for (const auto& neighbor : neighbors) {
        if (neighbor->isTagged()) {
            averageHeading += neighbor->heading;
            ++neighborCount;
        }
    }

    // jeśli sąsiedztwo zawiera jeden lub więcej botów, uśrednij ich wektory kierunku
    if (neighborCount > 0) {
        averageHeading /= static_cast<float>(neighborCount);
        averageHeading -= parent->heading;
    }

    return averageHeading;
}

// --- cohesion ---

// kieruje bota w stronę centrum masy grupy sąsiadów (group behaviour)
sf::Vector2f SteeringBehaviours::cohesion(const std::vector<std::unique_ptr<Enemy>>& neighbors) const {
    if (!parent)
        return {0, 0};

    sf::Vector2f centerOfMass(0, 0);
    sf::Vector2f steeringForce(0, 0);
    int neighborCount = 0;

    for (const auto& neighbor : neighbors) {
        if (neighbor->isTagged()) {
            centerOfMass += neighbor->getPosition();
            ++neighborCount;
        }
    }

    if (neighborCount > 0) {
        // centrum masy to średnia z sumy pozycji
        centerOfMass /= static_cast<float>(neighborCount);

        // teraz seek w kierunku tej pozycji
        steeringForce = seek(centerOfMass);
    }

    return steeringForce;
}
