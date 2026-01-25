#include "rocket_launcher.h"
#include "../../bots/bot.h"



RocketLauncher::RocketLauncher() {
	damage = Parameters::ROCKET_DAMAGE;
	ammo = Parameters::ROCKET_RESPAWN_AMMO;
	cooldown = Parameters::ROCKET_RELOAD_TIME;
}

void RocketLauncher::addAmmo(int amount) {
	ammo += amount;
	if (ammo > Parameters::ROCKET_MAX_AMMO) {
		ammo = Parameters::ROCKET_MAX_AMMO;
	}
}

void RocketLauncher::shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) {
	if (!canShoot())
		return;

	Weapon::shoot(shooterPos, direction, shooter, allBots);

	// zespawnuj i wystrzel rakiete
	auto projectile = std::make_unique<Projectile>(
		shooterPos,
		direction,
		Parameters::ROCKET_SPEED,
		Parameters::ROCKET_DAMAGE,
		Parameters::ROCKET_EXPLOSION_RADIUS,
		shooter,
		mapRef
	);

	projectiles.push_back(std::move(projectile));
	allBotsRef = allBots;

	std::cout << "Rocket fired from (" << shooterPos.x << ", " << shooterPos.y << ") in direction (" << direction.x << ", " << direction.y << ")\n";
}

void RocketLauncher::draw(sf::RenderWindow& window) {
	for (auto& projectile : projectiles) {
		projectile->draw(window);
	}
	for (auto& explosion : explosions) {
		explosion->draw(window);
	}
}

void RocketLauncher::update(float deltaTime) {
	Weapon::update(deltaTime);

	// zauktualizuj wszystkie pociski
	for (auto proj = projectiles.begin(); proj != projectiles.end(); ) {
		(*proj)->update(deltaTime, allBotsRef, &explosions);

		if ((*proj)->hasExploded()) {
			proj = projectiles.erase(proj);
		} else {
			++proj;
		}
	}

	// zauktualizuj wszystkie wybuchy
	for (auto expl = explosions.begin(); expl != explosions.end(); ) {
		(*expl)->update(deltaTime);

		if ((*expl)->isFinished()) {
			expl = explosions.erase(expl);
		} else {
			++expl;
		}
	}
}