#include "fsm.h"
#include "../../bots/bot.h"
#include "../../game/game.h"
#include "../../pickups/pickup.h"
#include "../../map/map.h"
#include "../../map/obstacle.h"
#include <cmath>
#include <iostream>



FSM::FSM() : currentState(BotState::RoamRandomly) {
	createState(currentState);
}

FSM::~FSM() = default;

void FSM::initialize(Bot* owner) {}

void FSM::update(Bot* bot, float deltaTime) {
	if (!stateImplementation)
		return;

	// pobierz zmysly
	BotSenseContext senses = gatherSenses(bot);

	// zaktualizuj stan na podstawie zmyslow
	stateImplementation->update(bot, senses, deltaTime);

	// przelacz stan na podstawie zmyslow
	BotState nextState = currentState;
	switch (currentState) {
	case BotState::RoamRandomly:
		// Roam -> Fight (znalazl wroga, ma zasoby)
		if (senses.enemyDetected && senses.hasManyResources) {
			nextState = BotState::Fight;
		}
		// Roam -> Fight to the End (znalazl wroga blisko, ma troche zasobow)
		else if (senses.enemyDetected && senses.hasLowResources && senses.isEnemyCloseRange) {
			nextState = BotState::FightToTheEnd;
		}
		// Roam -> Run Away (znalazl wroga, brak zasobow)
		else if (senses.enemyDetected && (senses.hasNoAmmo || (senses.hasLowResources && !senses.isEnemyCloseRange))) {
			nextState = BotState::RunAway;
		}
		// Roam -> Look for Resources (nie widzi wroga, nie ma zasobow)
		else if (!senses.hasManyResources) {
			nextState = BotState::LookForResources;
		}
		break;

	case BotState::LookForResources:
		// Look -> Fight (znalazl wroga, ma zasoby)
		if (senses.enemyDetected && senses.hasManyResources && !senses.isEnemyCloseRange) {
			nextState = BotState::Fight;
		}
		// Look -> Fight to the End (malo zasobow, wrog blisko)
		else if (senses.enemyDetected && senses.hasLowResources && senses.isEnemyCloseRange) {
			nextState = BotState::FightToTheEnd;
		}
		// Look -> Run Away (brak zasobow, znalazl wroga)
		else if (senses.enemyDetected && (senses.hasNoAmmo || (senses.hasLowResources && !senses.isEnemyCloseRange))) {
			nextState = BotState::RunAway;
		}
		// Look -> Roam (znalazl zasoby)
		else if (senses.hasManyResources && !senses.pickupDetected) {
			nextState = BotState::RoamRandomly;
		}
		break;

	case BotState::Fight:
		// Fight -> Fight to the End (wrog blisko)
		if (senses.enemyDetected && senses.hasLowResources && senses.isEnemyCloseRange) {
			nextState = BotState::FightToTheEnd;
		}
		// Fight -> Run Away (walczy, brak zasobow)
		else if (senses.enemyDetected && (senses.hasNoAmmo || (senses.hasLowResources && !senses.isEnemyCloseRange))) {
			nextState = BotState::RunAway;
		}
		// Fight -> Look for Resources (nie widzi wroga, malo zasobow)
		else if (!senses.enemyDetected && !senses.hasManyResources) {
			nextState = BotState::LookForResources;
		}
		// Fight -> Roam (nie widzi wroga, ma zasoby)
		else if (!senses.enemyDetected) {
			nextState = BotState::RoamRandomly;
		}
		break;

	case BotState::FightToTheEnd:
		// Fight to End -> Run Away (widzi wroga, brak zasobow)
		if (senses.enemyDetected && senses.hasNoAmmo) {
			nextState = BotState::RunAway;
		}
		// Fight to End -> Look (nie widzi wroga)
		else if (!senses.enemyDetected) {
			nextState = BotState::LookForResources;
		}
		break;

	case BotState::RunAway:
		// Run Away -> Look for Resources (nie widzi wroga, brak zasobow)
		if (!senses.enemyDetected && !senses.hasManyResources) {
			nextState = BotState::LookForResources;
		}
		// Run Away -> Roam (nie widzi wroga, ma zasoby)
		else if (!senses.enemyDetected) {
			nextState = BotState::RoamRandomly;
		}
		break;
	}

	// Zmien stan jesli potrzeba
	if (nextState != currentState) {
		setState(nextState);
	}
}

void FSM::setState(BotState newState) {
	if (newState == currentState)
		return;

	currentState = newState;
	createState(newState);
}

BotState FSM::getCurrentState() const {
	return currentState;
}

BotSenseContext FSM::gatherSenses(Bot* bot) const {
	BotSenseContext senses;

	if (!bot)
		return senses;

	// sprawdz zasoby
	senses.hasManyResources = hasManyResources(bot);
	senses.hasLowResources = hasLowResources(bot);
	senses.hasNoAmmo = hasNoAmmo(bot);

	// sprawdz wrogow w zasiegu
	senses.enemyDetected = canSeeEnemy(bot, senses.detectedEnemy, senses.enemyDistance);
	senses.isEnemyCloseRange = senses.enemyDetected && isEnemyCloseRange(senses.detectedEnemy, senses.enemyDistance);

	// sprawdz pickupy w zasiegu
	senses.pickupDetected = (canSeePickup(bot, senses.pickupDistance) != nullptr);
	senses.detectedPickup = canSeePickup(bot, senses.pickupDistance);

	return senses;
}

void FSM::createState(BotState state) {
	switch (state) {
	case BotState::RoamRandomly:
		stateImplementation = std::make_unique<RoamRandomlyState>();
		break;
	case BotState::LookForResources:
		stateImplementation = std::make_unique<LookForResourcesState>();
		break;
	case BotState::Fight:
		stateImplementation = std::make_unique<FightState>();
		break;
	case BotState::FightToTheEnd:
		stateImplementation = std::make_unique<FightToTheEndState>();
		break;
	case BotState::RunAway:
		stateImplementation = std::make_unique<RunAwayState>();
		break;
	}
}
bool FSM::hasManyResources(Bot* bot) const {
	if (!bot)
		return false;

	// czy duzo ammo i HP
	int totalAmmo = bot->railGun.getAmmo() + bot->rocketLauncher.getAmmo();
	float health = bot->health.getHealth();

	return (totalAmmo > Parameters::MIN_AMMO_THRESHOLD) && (health > Parameters::MIN_HP_THRESHOLD);
}

bool FSM::hasLowResources(Bot* bot) const {
	if (!bot)
		return false;

	// czy malo ammo i HP
	int totalAmmo = bot->railGun.getAmmo() + bot->rocketLauncher.getAmmo();
	float health = bot->health.getHealth();

	return (totalAmmo <= Parameters::MIN_AMMO_THRESHOLD && totalAmmo > 0) || (health <= Parameters::MIN_HP_THRESHOLD);
}

bool FSM::hasNoAmmo(Bot* bot) const {
    if (!bot)
        return false;
    
	// czy brak ammo
    int totalAmmo = bot->railGun.getAmmo() + bot->rocketLauncher.getAmmo();
    return totalAmmo <= 0;
}

bool FSM::canSeeEnemy(Bot* bot, Bot*& outEnemy, float& outDistance) const {
	if (!bot || !bot->game)
		return false;

	float closestDistance = Parameters::VISION_RANGE;
	Bot* closestEnemy = nullptr;

	// sprawdz czy widzi jakies boty
	for (auto& otherBot : bot->game->bots) {
		if (otherBot.get() == bot)
			continue;

		float dist = distanceTo(bot->getPosition(), otherBot->getPosition());
		
		// czy bot jest w zasiegu i linii wzroku
		if (dist < Parameters::VISION_RANGE && hasLineOfSight(bot->getPosition(), otherBot->getPosition(), bot)) {
			if (dist < closestDistance) {
				closestDistance = dist;
				closestEnemy = otherBot.get();
			}
		}
	}

	if (closestEnemy) {
		outEnemy = closestEnemy;
		outDistance = closestDistance;
		return true;
	}

	return false;
}

Pickup* FSM::canSeePickup(Bot* bot, float& outDistance) const {
	if (!bot || !bot->game)
		return nullptr;

	float closestDistance = Parameters::VISION_RANGE;
	Pickup* closestPickup = nullptr;

	// sprawdz pickupy
	for (auto& pickup : bot->game->pickups) {
		if (!pickup->isActive())
			continue;

		float dist = distanceTo(bot->getPosition(), pickup->getPosition());

		// sprawdz czy pickup w zasiegu i linii wzroku
		if (dist < Parameters::PICKUP_DETECTION_RANGE && hasLineOfSight(bot->getPosition(), pickup->getPosition(), bot)) {
			if (dist < closestDistance) {
				closestDistance = dist;
				closestPickup = pickup.get();
			}
		}
	}

	if (closestPickup) {
		outDistance = closestDistance;
		return closestPickup;
	}

	return nullptr;
}

bool FSM::isEnemyCloseRange(const Bot* enemy, float distance) const {
	return distance < Parameters::CLOSE_RANGE;
}

float FSM::distanceTo(const sf::Vector2f& from, const sf::Vector2f& to) const {
	float dx = to.x - from.x;
	float dy = to.y - from.y;
	return std::sqrt(dx * dx + dy * dy);
}

sf::Vector2f FSM::directionTo(const sf::Vector2f& from, const sf::Vector2f& to) const {
	sf::Vector2f dir = to - from;
	float len = distanceTo(from, to);
	if (len > 0.f)
		dir /= len;
	return dir;
}

bool FSM::hasLineOfSight(const sf::Vector2f& from, const sf::Vector2f& to, Bot* ignoreBotSelf) const {
	if (!ignoreBotSelf || !ignoreBotSelf->game || !ignoreBotSelf->game->map)
		return true; // jesli nie ma bota lub gry, zakladamy true

	// sprawdz wszystkie przeszkody na mapie
	const auto& obstacles = ignoreBotSelf->game->map->getObstacles();
	for (const auto& obstacle : obstacles) {
		// Check if the line segment from 'from' to 'to' intersects with this obstacle
		if (lineIntersectsObstacle(from, to, obstacle)) {
			return false; // wykryto przeszkode
		}
	}

	return true; // czysto
}

bool FSM::lineIntersectsObstacle(const sf::Vector2f& from, const sf::Vector2f& to, const Obstacle& obstacle) const {
	const auto& vertices = obstacle.getVertices();
	if (vertices.size() < 3)
		return false;

	// czy przecina się z krawedziami przeszkody
	for (size_t i = 0; i < vertices.size(); ++i) {
		size_t nextI = (i + 1) % vertices.size();
		if (lineSegmentsIntersect(from, to, vertices[i], vertices[nextI])) {
			return true;
		}
	}

	// sprawdz tez czy punkt poczatkowy jest wewnatrz przeszkody
	if (pointInPolygon(from, vertices)) {
		return true;
	}

	return false;
}

bool FSM::lineSegmentsIntersect(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4) const {
	// czy punkty p1p2 (linia wzroku) i p3p4 (krawedz przeszkody) sie przecinaja
	auto ccw = [](const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2f& C) {
		return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
	};

	return ccw(p1, p3, p4) != ccw(p2, p3, p4) && ccw(p1, p2, p3) != ccw(p1, p2, p4);
}

bool FSM::pointInPolygon(const sf::Vector2f& point, const std::vector<sf::Vector2f>& vertices) const {
	if (vertices.size() < 3)
		return false;

	bool inside = false;
	size_t j = vertices.size() - 1;

	for (size_t i = 0; i < vertices.size(); ++i) {
		const sf::Vector2f& vi = vertices[i];
		const sf::Vector2f& vj = vertices[j];

		if ((vi.y > point.y) != (vj.y > point.y) &&
			point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x) {
			inside = !inside;
		}
		j = i;
	}

	return inside;
}

std::string FSM::getCurrentStateName() const {
    switch (currentState) {
    case BotState::RoamRandomly:
        return "Roam";
    case BotState::LookForResources:
        return "Look";
    case BotState::Fight:
        return "Fight";
    case BotState::FightToTheEnd:
        return "ToTheEnd";
    case BotState::RunAway:
        return "Run";
    default:
        return "n/a";
    }
}



// implementacje stanow FSM

// implementacja RoamRandomlyState
void RoamRandomlyState::enter(Bot* bot) {
	if (!bot)
		return;
	bot->selectRandomNode();
}

void RoamRandomlyState::update(Bot* bot, const BotSenseContext& senses, float deltaTime) {
	if (!bot)
		return;

	// idz po sciezce
	bot->followPath(deltaTime);
	roamTimer += deltaTime;

	// po dojsciu znajdz nowy cel
	if (bot->hasArrived() || roamTimer > Parameters::ROAM_CHANGE_INTERVAL) {
		bot->selectRandomNode();
		roamTimer = 0.f;
	}
}



// implementacja LookForResourcesState
void LookForResourcesState::enter(Bot* bot) {
	if (!bot)
		return;
	targetPickup = nullptr;
	searchTimer = 0.f;
    bot->selectRandomNode();
}

void LookForResourcesState::update(Bot* bot, const BotSenseContext& senses, float deltaTime) {
	if (!bot)
		return;

    // jak widzi pickup to idz do niego
    if (senses.pickupDetected && senses.detectedPickup) {
        // tylko ustaw nowy cel jesli to inny pickup
        if (targetPickup != senses.detectedPickup) {
            targetPickup = senses.detectedPickup;
            sf::Vector2f targetNode = bot->getClosestNodePosition(senses.detectedPickup->getPosition());
            bot->setTargetNode(targetNode);
        }
        
        bot->followPath(deltaTime);
        return;
    }

	// nie widzi pickupa - losowy cel na mapie
	bot->followPath(deltaTime);
	searchTimer += deltaTime;

    // po dojsciu nowy cel
	if (bot->hasArrived() || searchTimer > Parameters::SEARCH_INTERVAL) {
		bot->selectRandomNode();
        searchTimer = 0.f;
	}
}



// implementacja FightState
void FightState::enter(Bot* bot) {
	if (!bot)
		return;
	targetEnemy = nullptr;
	pursuitTimer = 0.f;
}

void FightState::update(Bot* bot, const BotSenseContext& senses, float deltaTime) {
	if (!bot)
		return;

	if (senses.enemyDetected) {
		targetEnemy = senses.detectedEnemy;
		pursuitTimer = 0.f;

		// podejdz do wroga
		sf::Vector2f targetNode = bot->getClosestNodePosition(senses.detectedEnemy->getPosition());
		auto pathToEnemy = bot->game->navGraph->findPath(bot->getPosition(), targetNode);
		if (!pathToEnemy.empty()) {
			bot->followPath(deltaTime);
		}

		// oblicz kierunek do strzału
		sf::Vector2f direction = senses.detectedEnemy->getPosition() - bot->getPosition();
		float dirLen = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		if (dirLen > 0.f) {
			direction /= dirLen;
		}
		
		// niedokladnosc strzału podczas walki
        float inaccuracy = Parameters::SHOT_INACCURACY;
		std::uniform_real_distribution<float> inaccuracyDist(-inaccuracy, inaccuracy);
		float angleDeviation = inaccuracyDist(Game::gen);
		float cosA = std::cos(angleDeviation);
		float sinA = std::sin(angleDeviation);
		sf::Vector2f inaccurateDir(
			direction.x * cosA - direction.y * sinA,
			direction.x * sinA + direction.y * cosA
		);
		
		// zmien bron zaleznie od ammo
		if (bot->rocketLauncher.canShoot() && senses.enemyDistance < Parameters::ROCKET_RANGE) {
			// celuj rakietami z przewidywaniem pozycji wroga
			sf::Vector2f leadPosition = senses.detectedEnemy->getPosition();
			float flightTime = senses.enemyDistance / Parameters::ROCKET_SPEED;
			leadPosition += (senses.detectedEnemy->getPosition() - bot->getPosition()) * 0.4f;
			
			sf::Vector2f leadDirection = leadPosition - bot->getPosition();
			float leadLen = std::sqrt(leadDirection.x * leadDirection.x + leadDirection.y * leadDirection.y);
			if (leadLen > 0.f) {
				leadDirection /= leadLen;
			}
			
			// niedokladnosc tez dla rakiet
			float leadAngleDeviation = inaccuracyDist(Game::gen);
			float leadCosA = std::cos(leadAngleDeviation);
			float leadSinA = std::sin(leadAngleDeviation);
			sf::Vector2f inaccurateLeadDir(
				leadDirection.x * leadCosA - leadDirection.y * leadSinA,
				leadDirection.x * leadSinA + leadDirection.y * leadCosA
			);
			
			bot->rocketLauncher.shoot(bot->getPosition(), inaccurateLeadDir, bot, &bot->game->bots);
		}
		else if (bot->railGun.canShoot()) {
			bot->railGun.shoot(bot->getPosition(), inaccurateDir, bot, &bot->game->bots);
		}
	}
	else {
		pursuitTimer += deltaTime;
		if (pursuitTimer > Parameters::PURSUIT_TIMEOUT) {
			targetEnemy = nullptr;
		}
		else {
			// idz do ostatniej pozycji wroga
			bot->followPath(deltaTime);
		}
	}
}



// implementacja FightToTheEndState
void FightToTheEndState::enter(Bot* bot) {
	if (!bot)
		return;
	targetEnemy = nullptr;
	targetLossTimer = 0.f;
}

void FightToTheEndState::update(Bot* bot, const BotSenseContext& senses, float deltaTime) {
	if (!bot)
		return;

	if (senses.enemyDetected) {
		targetEnemy = senses.detectedEnemy;
		targetLossTimer = 0.f;

        // podejdz do wroga
		sf::Vector2f targetNode = bot->getClosestNodePosition(senses.detectedEnemy->getPosition());
		auto pathToEnemy = bot->game->navGraph->findPath(bot->getPosition(), targetNode);
		if (!pathToEnemy.empty()) {
			bot->followPath(deltaTime);
		}

		// oblicz kierunek do strzału
		sf::Vector2f direction = senses.detectedEnemy->getPosition() - bot->getPosition();
		float dirLen = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		if (dirLen > 0.f) {
			direction /= dirLen;
		}

	    // niedokladnosc strzału podczas walki
        float inaccuracy = Parameters::SHOT_INACCURACY;
		std::uniform_real_distribution<float> inaccuracyDist(-inaccuracy, inaccuracy);
		float angleDeviation = inaccuracyDist(Game::gen);
		float cosA = std::cos(angleDeviation);
		float sinA = std::sin(angleDeviation);
		sf::Vector2f inaccurateDir(
			direction.x * cosA - direction.y * sinA,
			direction.x * sinA + direction.y * cosA
		);
		

		// Uzywaj wszystkich broni 
		if (bot->rocketLauncher.canShoot()) {
			sf::Vector2f leadPosition = senses.detectedEnemy->getPosition();
			leadPosition += (senses.detectedEnemy->getPosition() - bot->getPosition()) * 0.4f;
			
			sf::Vector2f leadDirection = leadPosition - bot->getPosition();
			float leadLen = std::sqrt(leadDirection.x * leadDirection.x + leadDirection.y * leadDirection.y);
			if (leadLen > 0.f) {
				leadDirection /= leadLen;
			}
			
			// Apply inaccuracy to lead direction
			float leadAngleDeviation = inaccuracyDist(Game::gen);
			float leadCosA = std::cos(leadAngleDeviation);
			float leadSinA = std::sin(leadAngleDeviation);
			sf::Vector2f inaccurateLeadDir(
				leadDirection.x * leadCosA - leadDirection.y * leadSinA,
				leadDirection.x * leadSinA + leadDirection.y * leadCosA
			);
			
			bot->rocketLauncher.shoot(bot->getPosition(), inaccurateLeadDir, bot, &bot->game->bots);
		}
		else if (bot->railGun.canShoot()) {
			bot->railGun.shoot(bot->getPosition(), inaccurateDir, bot, &bot->game->bots);
		}
	}
	else {
		targetLossTimer += deltaTime;
		if (targetLossTimer > Parameters::TARGET_LOSS_TIMEOUT) {
			targetEnemy = nullptr;
		}
	}
}



// implementacja RunAwayState
void RunAwayState::enter(Bot* bot) {
	if (!bot)
		return;
	threateningEnemy = nullptr;
	fleeTimer = 0.f;
	fleeDirection = sf::Vector2f(1.f, 0.f); // fallback kierunek
	lastFleeTarget = sf::Vector2f(0.f, 0.f);
}

void RunAwayState::update(Bot* bot, const BotSenseContext& senses, float deltaTime) {
	if (!bot)
		return;

	fleeTimer += deltaTime;

	if (senses.enemyDetected) {
		threateningEnemy = senses.detectedEnemy;
		fleeTimer = 0.f;

		// oblicz kierunek przeciwnie od wroga
		sf::Vector2f toEnemy = threateningEnemy->getPosition() - bot->getPosition();
		float len = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
		if (len > 0.f) {
			toEnemy /= len;
			fleeDirection = sf::Vector2f(-toEnemy.x, -toEnemy.y);
		}
	}

	// uciekaj od wroga
	if (fleeTimer < Parameters::FLEE_DURATION) {
		// Oblicz nowy cel ucieczki
		sf::Vector2f newFleeTarget = bot->getPosition() + fleeDirection * 200.f;
		
		// Tylko odświeżaj ścieżkę jeśli znacznie się oddaliliśmy od ostatniego celu
		float distToLastTarget = std::sqrt(
			(newFleeTarget.x - lastFleeTarget.x) * (newFleeTarget.x - lastFleeTarget.x) +
			(newFleeTarget.y - lastFleeTarget.y) * (newFleeTarget.y - lastFleeTarget.y)
		);
		
		if (distToLastTarget > 50.f || lastFleeTarget.x == 0.f) {
			lastFleeTarget = newFleeTarget;
			sf::Vector2f targetNode = bot->getClosestNodePosition(newFleeTarget);
			bot->setTargetNode(targetNode);
		}

		bot->followPath(deltaTime);
		
		// Jeśli nie ma ścieżki, uciekaj w linii prostej
		if (bot->hasArrived()) {
			bot->setPosition(bot->getPosition() + fleeDirection * Parameters::BOT_SPEED * deltaTime);
		}
	}
}

