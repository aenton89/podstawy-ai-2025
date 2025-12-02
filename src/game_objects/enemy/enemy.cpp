#include "enemy.h"
#include "../../game/game.h"
#include "../../helpers/parameters.h"
#include "../../helpers/helper_methods.h"



Enemy::Enemy(float _x, float _y, Player* _player): GameObject(_x, _y, ENEMY_SIZE) {
	shape.setRadius(ENEMY_SIZE);
	shape.setOrigin(ENEMY_SIZE, ENEMY_SIZE);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition(_x, _y);

	steering.setParent(this);
	player = _player;

	idleTimeTreshold = std::uniform_real_distribution<float>(MIN_IDLE_TIME_THRESHOLD, MAX_IDLE_TIME_THRESHOLD)(Game::gen);
}

void Enemy::updateColliderPosition() {
	collider.position = { shape.getPosition().x, shape.getPosition().y };
}

void Enemy::update(float dt, sf::RenderWindow& window) {
	steering_force = steering.calculate();
	velocity += steering_force;

	// RANDOM WANDER CONTROL
	randomWanderSwitch(dt);
	if (getState() == State::RandomWander) {
		// sprawdź ile minęło czasu
		float elapsed = randomWanderClock.getElapsedTime().asSeconds();

		if (elapsed >= randomWanderDuration) {
			setState(State::HideExplore);
		}
	}


	// ograniczenie prędkości
	auto length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (length > MAX_ENEMY_SPEED)
		velocity = (velocity / length) * MAX_ENEMY_SPEED;

	// liczenie heading i side
	if (length > 0.0000001f)
		heading = normalizeVec2D(velocity);
	else
		heading = sf::Vector2f(1.f,0.f);
	side = sf::Vector2f(-heading.y, heading.x);

	// aktualizacja pozycji i ostatniej pozycji
	prevPosition = getPosition();
	shape.move(velocity*dt);
	updateColliderPosition();
}

void Enemy::tag() {
	tagged = true;
}

void Enemy::unTag() {
	tagged = false;
}

bool Enemy::isTagged() const {
	return tagged;
}

void Enemy::setState(State _state) {
	currentState = _state;
	if (currentState == State::HideExplore)
		shape.setFillColor(sf::Color::Green);
	else if (currentState == State::Attack)
		shape.setFillColor(sf::Color::Blue);
	else if (currentState == State::RandomWander)
		shape.setFillColor(sf::Color::Magenta);
}

State Enemy::getState() const {
	return currentState;
}

// metoda wykrywa pobliskich agentów i wysyła ich listę do Game
std::vector<Enemy*> Enemy::getNeighbours(const std::vector<std::unique_ptr<Enemy>>& neighbors, float radius) const {
    std::vector<Enemy*> result;

    for (auto& neighbor : neighbors) {
    	if (neighbor.get() == this)
    		continue;

        sf::Vector2f to = neighbor->getPosition() - this->getPosition();
        float distSq = to.x * to.x + to.y * to.y;

        float range = radius + neighbor->collider.radius;

        if (distSq < range * range)
            result.push_back(neighbor.get());
    }

    return result;
}

// sprawdza czy wejść do stanu RandomWander, jeśli tak to wchodzi i zaczyna timer + losuje czas trwania stanu
void Enemy::randomWanderSwitch(float dt) {
	if (getState() == State::HideExplore) {
		// jesli za duzo się nie poruszył w ostatnim czasie, to wchodzimy w randomWander
		auto distMoved = distanceVec2D(getPosition(), prevPosition);

		if (distMoved < 0.5f)
			idleTimer += dt;
		else
			idleTimer = 0.f;

		if (idleTimer >= idleTimeTreshold) {
			idleTimer = 0.f;
			idleTimeTreshold = std::uniform_real_distribution<float>(MIN_IDLE_TIME_THRESHOLD, MAX_IDLE_TIME_THRESHOLD)(Game::gen);

			randomWanderDuration = std::uniform_real_distribution<float>(RANDOM_WANDER_MIN_TIME, RANDOM_WANDER_MAX_TIME)(Game::gen);
			randomWanderClock.restart();

			setState(State::RandomWander);
		}
	}
}

bool Enemy::checkRandomWanderTimeout() {
	if (getState() == State::RandomWander) {
		if (randomWanderClock.getElapsedTime().asSeconds() >= randomWanderDuration) {
			// wracamy do HideExplore
			setState(State::HideExplore);
			return true;
		}
	}
	return false;
}



