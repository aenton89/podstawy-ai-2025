#include "rail_gun.h"
#include "../../bots/bot.h"



RailGun::RailGun() {
	damage = Parameters::RAILGUN_DAMAGE;
	ammo = Parameters::RAILGUN_MAX_AMMO;
	cooldown = Parameters::RAILGUN_RELOAD_TIME;
}

void RailGun::addAmmo(int amount) {
	ammo += amount;
	if (ammo > Parameters::RAILGUN_MAX_AMMO) {
		ammo = Parameters::RAILGUN_MAX_AMMO;
	}
}

void RailGun::shoot(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) {
	if (!canShoot())
		return;

	Weapon::shoot(shooterPos, direction, shooter, allBots);

	// ustaw wizualizację promienia
	beamStart = shooterPos;
	beamEnd = shooterPos + direction * Parameters::RAILGUN_RANGE;
	beamActive = true;
	beamTimer = beamDuration;

	checkHit(shooterPos, direction, shooter, allBots);
}

void RailGun::checkHit(const sf::Vector2f& shooterPos, const sf::Vector2f& direction, Bot* shooter, std::vector<std::unique_ptr<Bot>>* allBots) {
	std::cout<< "RailGun fired from (" << shooterPos.x << ", " << shooterPos.y << ") in direction (" << direction.x << ", " << direction.y << ")\n";

	// railgun to hitscan - sprawdzamy czy promień trafia w któregoś bota
	Bot* hitBot = nullptr;
	float closestDist = Parameters::RAILGUN_RANGE;

	for (auto& bot : *allBots) {
		// nie strzelamy w siebie
		if (bot.get() == shooter)
			continue;

		sf::Vector2f toBot = bot->getPosition() - shooterPos;
		float distToBot = std::sqrt(toBot.x * toBot.x + toBot.y * toBot.y);

		if (distToBot > Parameters::RAILGUN_RANGE)
			continue;

		// normalizuj
		toBot /= distToBot;

		// sprawdź czy bot jest w kierunku strzału (dot product)
		float dot = toBot.x * direction.x + toBot.y * direction.y;

		// jeśli kierunek się zgadza (dot > 0.99 = ~8 stopni)
		if (dot > 0.99f && distToBot < closestDist) {
			// sprawdź dokładną odległość od promienia
			sf::Vector2f toBotVec = bot->getPosition() - shooterPos;
			float projection = toBotVec.x * direction.x + toBotVec.y * direction.y;
			sf::Vector2f closestPoint = shooterPos + direction * projection;
			sf::Vector2f diff = bot->getPosition() - closestPoint;
			float distFromRay = std::sqrt(diff.x * diff.x + diff.y * diff.y);

			// jeśli promień przechodzi przez bota
			if (distFromRay < Parameters::BOT_RADIUS) {
				closestDist = distToBot;
				hitBot = bot.get();
			}
		}
	}

	if (hitBot) {
		dealDamage(hitBot);
		std::cout << "RailGun HIT! Target HP: " << hitBot->health.getHealth() << std::endl;
	} else {
		std::cout << "RailGun MISS!" << std::endl;
	}
}

void RailGun::draw(sf::RenderWindow& window) {
	if (!beamActive)
		return;

	// rysuj linię promienia - cyan z przezroczystością, zanikający koniec
	sf::Vertex line[] = {
		sf::Vertex(beamStart, sf::Color(0, 255, 255, 200)),
		sf::Vertex(beamEnd, sf::Color(0, 255, 255, 50))
	};
	window.draw(line, 2, sf::Lines);

	// opcjonalnie: narysuj grubszą linię dla efektu
	sf::RectangleShape beam;
	sf::Vector2f direction = beamEnd - beamStart;
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f;

	beam.setSize(sf::Vector2f(length, 3.f));
	beam.setPosition(beamStart);
	beam.setRotation(angle);
	beam.setFillColor(sf::Color(0, 255, 255, 150));
	window.draw(beam);
}

void RailGun::update(float deltaTime) {
	Weapon::update(deltaTime);

	if (beamActive) {
		beamTimer -= deltaTime;
		if (beamTimer <= 0.f)
			beamActive = false;
	}
}
